#include <stdio.h>
#include <ros.h>
#include <std_msgs/Float32.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/Vector3.h>
#include <geometry_msgs/Quaternion.h>
#include "FlashStorage_STM32.h"
#include "demo_serial.h"
#include "LED_KEY_TM1638.h"
#include "motion_mc_manager.h"
#include "app_mems.h"

#define MOTION_FX_ENGINE_DELTATIME 0.01f
#define FROM_MG_TO_G               0.001f
#define FROM_G_TO_MG               1000.0f
#define FROM_MDPS_TO_DPS           0.001f
#define FROM_DPS_TO_MDPS           1000.0f
#define FROM_MGAUSS_TO_UT50        ( 0.1f / 50.0f )
#define FROM_UT50_TO_MGAUSS        500.0f

// Порт для работы с внешней панелью ввода и индикации.
extern USART_HandleTypeDef husart1;
extern CRC_HandleTypeDef hcrc;

// Переменная для хранения данных с кнопок (бит соответствует нажатой клавише)
uint8_t keyPressed = 0;
uint8_t keyPressedlast = 0;
uint8_t leds_state = 0;

uint8_t  digit              = 0;
uint32_t imu_ticks_last     = 0;
uint32_t leds_ticks_last    = 0;
uint32_t magcal_ticks_last  = 0;
uint32_t halfsec_ticks_last = 0;
uint32_t keys_ticks_last    = 0;

static MOTION_SENSOR_Axes_t AccValue {0};
static MOTION_SENSOR_Axes_t GyrValue {0};
static MOTION_SENSOR_Axes_t MagValue {0};
static float PressValue = 0.f;
static float TempValue  = 0.f;
static float HumValue   = 0.f;

MFX_input_t fx_data_in {0};
MFX_output_t fx_data_out {0};

// Объявляем необходимые типы сообщений под каждый из датчиков.
std_msgs::Float32 temp_msg, hum_msg, press_msg, heading_msg, headingErr_msg;
geometry_msgs::Vector3 acc_msg, gyr_msg, mag_msg, rotation_msg, gravity_msg, linacc_msg;
geometry_msgs::Quaternion quaternion_msg;
sensor_msgs::Imu imu_msg;

ros::NodeHandle nh;

// Привязываем сообщения к публикуемым топикам.
ros::Publisher pub_acc( "nucleo/sensors/acc", &acc_msg );
ros::Publisher pub_gyr( "nucleo/sensors/gyr", &gyr_msg );
ros::Publisher pub_mag( "nucleo/sensors/mag", &mag_msg );
ros::Publisher pub_imu( "nucleo/imu/data", &imu_msg );

ros::Publisher pub_press( "nucleo/sensors/press", &press_msg );
ros::Publisher pub_temp( "nucleo/sensors/temp", &temp_msg );
ros::Publisher pub_hum( "nucleo/sensors/hum", &hum_msg );

ros::Publisher pub_quaternion( "nucleo/fusion/quaternion", &quaternion_msg );
ros::Publisher pub_rotation( "nucleo/fusion/rotation", &rotation_msg );
ros::Publisher pub_gravity( "nucleo/fusion/gravity", &gravity_msg );
ros::Publisher pub_linacc( "nucleo/fusion/linacc", &linacc_msg );
ros::Publisher pub_heading( "nucleo/fusion/heading", &heading_msg );
ros::Publisher pub_headingErr( "nucleo/fusion/headingErr", &headingErr_msg );

// NOTE: Must be implemented for each platform separately, because its implementation
//       is platform dependent. No need to call this function, library call this
//       function automatically.
/**
 * @brief  Load the calibration parameters from storage
 * @param  dataSize  size of data
 * @param  data  pointer to data
 * @retval Will return 0 the if it is success and 1 if it is failure
 */
char MotionMC_LoadCalFromNVM(unsigned short int datasize, unsigned int *data)
{
    if ( datasize > EEPROM.length() ) return HAL_ERROR;

    // Считываем данные калибровки магнетометра из флеш памяти.
    uint8_t value;
    uint8_t * bytes = ( uint8_t * ) data;

    for ( unsigned short int i = 0 ; i < datasize; i++ )
    {
        EEPROM.get( i, value );

        bytes[i] = value;
    }

    // Считываем значение CRC.
    uint32_t crc;
    EEPROM.get( datasize, crc );

    // Проверяем целостность данных.
    uint32_t word_count = ( datasize + 3 ) / 4;
    uint32_t crc_value = HAL_CRC_Calculate( &hcrc, ( uint32_t * ) data, word_count );

    return crc_value == crc ? HAL_OK : HAL_ERROR;
}

// NOTE: Must be implemented for each platform separately, because its implementation
//       is platform dependent. No need to call this function, library call this
//       function automatically.
/**
 * @brief  Save the calibration parameters in storage
 * @param  dataSize  size of data
 * @param  data  pointer to data
 * @retval Will return 0 the if it is success and 1 if it is failure
 */
char MotionMC_SaveCalInNVM(unsigned short int datasize, unsigned int *data)
{
    if ( datasize > EEPROM.length() ) return HAL_ERROR;

    // Запрещаем все прерывания.
    __disable_irq();

    // Отключаем функцию немедленной записи во флеш память.
    EEPROM.setCommitASAP( false );

    // Сохраняем результаты калибровки магнетометра.
    uint8_t * bytes = ( uint8_t * ) data;

    for ( unsigned short int i = 0 ; i < datasize; i++ )
    {
        EEPROM.put( i, bytes[i] );
    }

    // Подписываем данные при помощи значения CRC.
    uint32_t word_count = ( datasize + 3 ) / 4;
    uint32_t crc_value = HAL_CRC_Calculate( &hcrc, ( uint32_t * ) data, word_count );

    // Сохраняем значение CRC во флеш памяти.
    EEPROM.put( datasize, crc_value );

    // Сбрасываем данные на флеш память.
    EEPROM.commit();

    // Разрешаем прерывания.
    __enable_irq();

    return HAL_OK;
}


char MotionMC_ClearCalInNVM(unsigned short int datasize)
{
    if ( datasize > EEPROM.length() ) return HAL_ERROR;

    // Запрещаем все прерывания.
    __disable_irq();

    // Отключаем функцию немедленной записи во флеш память.
    EEPROM.setCommitASAP( false );

    for ( unsigned short int i = 0 ; i < datasize; i++ )
    {
        EEPROM.put( i, 0 );
    }

    // Сбрасываем данные на флеш память.
    EEPROM.commit();

    // Разрешаем прерывания.
    __enable_irq();

    return HAL_OK;
}

/**
  * @brief  Sensor Fusion data handler
  * @param  Msg the Sensor Fusion data part of the stream
  * @retval None
  */
static void FX_Data_Handler()
{
    if ( HAL_IS_BIT_SET( SensorsEnabled, ACCELEROMETER_SENSOR | GYROSCOPE_SENSOR | MAGNETIC_SENSOR ) )
    {
        // Convert acceleration from [mg] to [g].
        fx_data_in.acc[0] = (float) AccValue.x * FROM_MG_TO_G;
        fx_data_in.acc[1] = (float) AccValue.y * FROM_MG_TO_G;
        fx_data_in.acc[2] = (float) AccValue.z * FROM_MG_TO_G;

        // Convert angular velocity from [mdps] to [dps].
        fx_data_in.gyro[0] = (float) GyrValue.x * FROM_MDPS_TO_DPS;
        fx_data_in.gyro[1] = (float) GyrValue.y * FROM_MDPS_TO_DPS;
        fx_data_in.gyro[2] = (float) GyrValue.z * FROM_MDPS_TO_DPS;

        // Convert magnetic field intensity from [mGauss] to [uT/50].
        fx_data_in.mag[0] = (float) MagValue.x * FROM_MGAUSS_TO_UT50;
        fx_data_in.mag[1] = (float) MagValue.y * FROM_MGAUSS_TO_UT50;
        fx_data_in.mag[2] = (float) MagValue.z * FROM_MGAUSS_TO_UT50;
   
        // Запуск алгоритма Sensor Fusion.
        MotionFX_manager_run( &fx_data_in, &fx_data_out, MOTION_FX_ENGINE_DELTATIME );
    }
}


/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
    // Калибровка магнетометра.
    if ( GPIO_Pin == S1_Pin )
    {
        // Магнитометр не откалиброван.
        HAL_GPIO_WritePin( LED_D2_GPIO_Port, LED_D2_Pin, GPIO_PIN_SET );

        BSP_PB_Callback( BUTTON_USER );

        HAL_TIM_Base_Start_IT( &BSP_IP_TIM_Handle );
    }

    // Ввод магнитного склонения.
    if ( GPIO_Pin == S2_Pin )
    {
        //...
    }

    // Запись
    if ( GPIO_Pin == S3_Pin )
    {
        //...
    }
}


void TestIndicator()
{
    // Очищаем все полностью и светодиоды и семисегментные индикаторы.
    TM1638_Clear_All();

    // Включаем по очереди все светодиоды.
    for ( uint8_t i = 1; i <= 8; i++ )
    {
        // Включение и выключение светодиодов.
        // первый - номер светодиода от 1 до 8.
        // второй - состояние LED_OFF или LED_ON.
        TM1638_Led_OnOff( i, LED_ON );

        HAL_Delay( 200 );
    }

    // выключаем по очереди все светодиоды
    for ( uint8_t i = 1; i <= 8; i++ )
    {
        // включение и выключение светодиодов
        // первый - номер светодиода от 1 до 8
        // второй - состояние LED_OFF или LED_ON
        TM1638_Led_OnOff( i, LED_OFF );

        HAL_Delay( 200 );
    }

    // функция формирует одно разрядное число ( 1 цифру )
    // в указанном разряде ( можно совместно включить точку )
    // 1- позиция ( разряд ) где отобразить значение от 1 до 8
    // 2- сама цифра от 0 до 9 ( если нужен знак минус то ставим 11 )
    // 3- точка ( 1 включить,  0 выключить )
    TM1638_sendOneDigit( 1, 0, 0 );

    HAL_Delay( 1000 );

    // очищаем ( выключаем все семисегментные индикаторы )
    TM1638_Clear_SevenSegment();

    // функция для отображения любого числа на дисплее
    // строка которую нужно отобразить :
    // символы цифры от 0 до 9
    // знак минус ( может быть не один )
    // знак точка ( может быть не один )
    // Например: "-10.56" "0.56" "-2 -2.3" "1.2.3.4." "-23-" и т.д
    TM1638_sendNumber( (char*) "88888888" );

    HAL_Delay( 1000 );

    // очищаем ( выключаем все семисегментные индикаторы )
    TM1638_Clear_SevenSegment();

    // в цикле выводим числа от 0 до 9
    for ( int32_t i = 0; i < 10; i++ )
    {
        TM1638_Led_OnOff( 1, LED_ON );

        // очищаем ( выключаем все семисегментные индикаторы )
        TM1638_Clear_SevenSegment();

        // функция для отображения INT целочисленного числа на дисплее
        // число целочисленное :
        // цифры от -999 9999 до 9999 9999
        TM1638_sendNumberInt(i);

        HAL_Delay( 100 );

        TM1638_Led_OnOff( 1, LED_OFF );

        HAL_Delay( 100 );
    }

    HAL_Delay( 1000 );

    // очищаем (выключаем все семисегментные индикаторы).
    TM1638_Clear_SevenSegment();
}


/**
 * @brief   Выполняет начальные настройки.
 * 
 */
void init( void )
{
    // Магнитометр не откалиброван.
    HAL_GPIO_WritePin( LED_D2_GPIO_Port, LED_D2_Pin, GPIO_PIN_SET );

    // Настройка яркости дисплея.
    TM1638_init(2);

    // Очищаем все полностью светодиоды и семисегментные индикаторы.
    TM1638_Clear_All();

    nh.initNode();

    nh.getHardware()->init();

    nh.advertise( pub_acc );
    nh.advertise( pub_gyr );
    nh.advertise( pub_mag );

    nh.advertise( pub_press );
    nh.advertise( pub_temp );
    nh.advertise( pub_hum );

    nh.advertise( pub_quaternion );
    nh.advertise( pub_rotation );
    nh.advertise( pub_gravity );
    nh.advertise( pub_linacc );
    nh.advertise( pub_heading );
    nh.advertise( pub_headingErr );

    nh.advertise( pub_imu );
}


void SendToShiftRegisters( uint8_t segments, uint8_t digits )
{
    // Активируем LATCH (ST_CP) перед отправкой.
    HAL_GPIO_WritePin( LATCH_DIO_GPIO_Port, LATCH_DIO_Pin, GPIO_PIN_RESET );

    // Отправляем сначала сегменты (второй регистр), затем разряды (первый регистр)
    uint8_t data[2] = { segments, digits };

    HAL_USART_Transmit( &husart1, data, 2, HAL_MAX_DELAY );

    // "Защёлкиваем" данные на выходах регистров.
    HAL_GPIO_WritePin( LATCH_DIO_GPIO_Port, LATCH_DIO_Pin, GPIO_PIN_SET );
}


const uint8_t SEGMENT_CODES[] =
{
    0x03,    // 0 (00000011)
    0x9F,    // 1 (10011111)
    0x25,    // 2 (00100101)
    0x0D,    // 3 (00001101)
    0x99,    // 4 (10011001)
    0x49,    // 5 (01001001)
    0x41,    // 6 (01000001)
    0x1F,    // 7 (00011111)
    0x01,    // 8 (00000001)
    0x09     // 9 (00001001)
};


void DisplayNumber( uint16_t number, uint8_t n )
{
    uint8_t digits[4];

    digits[0] = number / 1000 % 10;    // Тысячи
    digits[1] = number / 100 % 10;     // Сотни
    digits[2] = number / 10 % 10;      // Десятки
    digits[3] = number % 10;           // Единицы

    // Маска разрядов (активируем по одному).
    const uint8_t DIGIT_MASKS[] = { 0x80, 0x40, 0x20, 0x10 };

    // Отправляем сегменты для текущей цифры и маску разряда.
    SendToShiftRegisters( SEGMENT_CODES[digits[n]], DIGIT_MASKS[n] );
}


/**
 * @brief   Публикует типики для ROS.
 * 
 */
void publish( void )
{
    char str[11];

    // Сканируем клавиатуру.
    if ( HAL_GetTick() - keys_ticks_last > 50 )
    {
        keyPressedlast = keyPressed;
        
        keyPressed = TM1638_readKey();

        // Если состояние клавиатуры изменилось.
        if ( keyPressedlast ^ keyPressed )
        {
            if ( keyPressed & 0x01 )
            {
                // Магнитометр не откалиброван.
                TM1638_Led_OnOff( 2, LED_OFF );

                BSP_PB_Callback( BUTTON_USER );

                HAL_TIM_Base_Start_IT( &BSP_IP_TIM_Handle );                
            }

            // Очистка калибровочных данных магнетометра.
            if ( keyPressed & 0x02 )
            {                
                MotionMC_ClearCalInNVM( 184 );

                // Enable magnetometer calibration.
                MotionMC_manager_MagCal_start( ALGO_PERIOD );
            }

            // Тестирование индикатора.
            if ( keyPressed & 0x80 )
            {
                TestIndicator();
            }
        }

        TM1638_Clear_SevenSegment();

        str[0] = '\0';

        sprintf( str, "%uo    ", ( uint16_t ) fx_data_out.heading );

        TM1638_sendNumber( str );
        //TM1638_sendNumberInt( ( uint32_t ) data_out.heading );

        keys_ticks_last = HAL_GetTick();
    }

    if ( HAL_GetTick() - halfsec_ticks_last > 500 )
    {
        BSP_LED_Toggle( LED2 );        

        if ( leds_state & 1 )
        {
            TM1638_Led_OnOff( 1, LED_OFF );
            CLEAR_BIT( leds_state, 1 );
        }
        else
        {
            TM1638_Led_OnOff( 1, LED_ON );
            SET_BIT( leds_state, 1 );
        }  

        halfsec_ticks_last = HAL_GetTick();
    }

    if ( HAL_GetTick() - leds_ticks_last > 5 )
    {
        //DisplayNumber( uint16_t( heading_msg.data ), digit++ % 4 );

        TM1638_Led_OnOff( 1, HAL_GPIO_ReadPin( LED2_GPIO_PORT, LED2_PIN ) == GPIO_PIN_RESET ? LED_ON : LED_OFF );

        leds_ticks_last = HAL_GetTick();
    }

    if ( HAL_GetTick() - imu_ticks_last > 25 )
    {
        // Принудительно включаем датчики.
        SensorsEnabled |= ACCELEROMETER_SENSOR | GYROSCOPE_SENSOR | MAGNETIC_SENSOR;

        BSP_SENSOR_ACC_Enable();
        BSP_SENSOR_GYR_Enable();
        BSP_SENSOR_MAG_Enable();

        BSP_SENSOR_TEMP_Enable();
        BSP_SENSOR_PRESS_Enable();
        BSP_SENSOR_HUM_Enable();

        // Устанавливаем режим работы с тремя датчиками (9X).
        if ( Enabled6X == 1U )
        {
            MotionFX_manager_stop_6X();
            MotionFX_manager_start_9X();

            Enabled6X = 0U;
        }

        MMC_Output_t mag_data_out {0};
        MOTION_SENSOR_Axes_t MagComp {0};

        // Получаем данные от датчиков.
        BSP_SENSOR_ACC_GetAxes( &AccValue );
        BSP_SENSOR_GYR_GetAxes( &GyrValue );
        BSP_SENSOR_MAG_GetAxes( &MagValue );

        MotionMC_manager_get_params( &mag_data_out );

        if ( mag_data_out.CalQuality == MMC_CALQSTATUSGOOD )
        {
            // Магнитометр откалиброван.
            HAL_GPIO_WritePin( LED_D2_GPIO_Port, LED_D2_Pin, GPIO_PIN_RESET );
            TM1638_Led_OnOff( 2, LED_ON );

            // Do hard & soft iron calibration.
            MotionMC_manager_compensate( &MagValue, &MagComp );

            MagValue.x = MagComp.x;
            MagValue.y = MagComp.y;
            MagValue.z = MagComp.z;
    
            if ( DataLoggerActive == 0 )
            {
                HAL_TIM_Base_Stop_IT( &BSP_IP_TIM_Handle );
            }
        }
        else
        {
            if ( BSP_IP_TIM_Handle.State == HAL_TIM_STATE_READY )
            {
                // Магнитометр не откалиброван.
                HAL_GPIO_WritePin( LED_D2_GPIO_Port, LED_D2_Pin, GPIO_PIN_SET );

                TM1638_Led_OnOff( 2, LED_OFF );
            }
            // Идёт процесс калибровки.
            else
            {
                if ( HAL_GetTick() - magcal_ticks_last > 250 )
                {
                    HAL_GPIO_TogglePin( LED_D2_GPIO_Port, LED_D2_Pin );

                    if ( leds_state & 2 )
                    {
                        TM1638_Led_OnOff( 2, LED_OFF );
                        CLEAR_BIT( leds_state, 2 );
                    }
                    else
                    {
                        TM1638_Led_OnOff( 2, LED_ON );
                        SET_BIT( leds_state, 2 );
                    }                    

                    magcal_ticks_last = HAL_GetTick();
                }
            }
        }

        // Объединяем данные датчиков.
        FX_Data_Handler();

        // Вектор ускорения, [g].
        acc_msg.x = fx_data_in.acc[0];
        acc_msg.y = fx_data_in.acc[1];
        acc_msg.z = fx_data_in.acc[2];

        // Вектор угловой скорости [deg/sec].
        gyr_msg.x = fx_data_in.gyro[0];
        gyr_msg.y = fx_data_in.gyro[1];
        gyr_msg.z = fx_data_in.gyro[2];

        // Калиброванный вектор магнитного поля, [mG].
        mag_msg.x = fx_data_in.mag[0];
        mag_msg.y = fx_data_in.mag[1];
        mag_msg.z = fx_data_in.mag[2];

        // Кватернион вращения (аналог углов Эйлера).
        quaternion_msg.x = fx_data_out.quaternion[0];
        quaternion_msg.y = fx_data_out.quaternion[1];
        quaternion_msg.z = fx_data_out.quaternion[2];
        quaternion_msg.w = fx_data_out.quaternion[3];

        // Углы Эйлера: yaw (рыскание), pitch (тангаж) and roll (крен).
        rotation_msg.x = fx_data_out.rotation[0];
        rotation_msg.y = fx_data_out.rotation[1];
        rotation_msg.z = fx_data_out.rotation[2];

        // Device frame gravity.
        gravity_msg.x = fx_data_out.gravity[0];
        gravity_msg.y = fx_data_out.gravity[1];
        gravity_msg.z = fx_data_out.gravity[2];

        // Device frame linear acceleration.
        linacc_msg.x = fx_data_out.linear_acceleration[0];
        linacc_msg.y = fx_data_out.linear_acceleration[1];
        linacc_msg.z = fx_data_out.linear_acceleration[2];

        // Магнитный курс.
        heading_msg.data = fx_data_out.heading;

        // Ошибка магнитного курса, [deg].
        headingErr_msg.data = fx_data_out.headingErr;

        imu_msg.header.stamp = nh.now();
        //imu_msg.header.frame_id = "base_imu_link";

        // Acceleration xyz includes gravity. *scaleAccel m/s is required when using it.
        imu_msg.linear_acceleration.x = fx_data_out.linear_acceleration[0];
        imu_msg.linear_acceleration.y = fx_data_out.linear_acceleration[1];
        imu_msg.linear_acceleration.z = fx_data_out.linear_acceleration[2];

        // Angular velocity xyz requires *scaleAngleSpeed ​​rad/s when used.
        imu_msg.angular_velocity.x = fx_data_in.gyro[0] * 0.0174532925; // unit rad/s
        imu_msg.angular_velocity.y = fx_data_in.gyro[1] * 0.0174532925; // unit rad/s
        imu_msg.angular_velocity.z = fx_data_in.gyro[2] * 0.0174532925; // unit rad/s

        // *scaleQuat is required when using the four-element wxyz.
        imu_msg.orientation.w = fx_data_out.quaternion[0];
        imu_msg.orientation.x = fx_data_out.quaternion[1];
        imu_msg.orientation.y = fx_data_out.quaternion[2];
        imu_msg.orientation.z = fx_data_out.quaternion[3];        

        if ( nh.connected() )
        {
            // Публикуем данные.
            pub_acc.publish( &acc_msg );
            pub_gyr.publish( &gyr_msg );
            pub_mag.publish( &mag_msg );

            pub_quaternion.publish( &quaternion_msg );
            pub_rotation.publish( &rotation_msg );
            pub_gravity.publish( &gravity_msg );
            pub_linacc.publish( &linacc_msg );
            pub_heading.publish( &heading_msg );
            pub_headingErr.publish( &headingErr_msg );

            BSP_SENSOR_PRESS_GetValue( &PressValue );
            BSP_SENSOR_TEMP_GetValue( &TempValue );
            BSP_SENSOR_HUM_GetValue( &HumValue );

            temp_msg.data = TempValue;
            press_msg.data = PressValue;
            hum_msg.data = HumValue;

            pub_temp.publish( &temp_msg );
            pub_press.publish( &press_msg );
            pub_hum.publish( &hum_msg );

            pub_imu.publish( &imu_msg );

            // Восстанавливаем флаги включения датчиков.
            //SensorsEnabled = oldSensorsEnabled;
        }

        nh.spinOnce();

        imu_ticks_last = HAL_GetTick();
    }
}
