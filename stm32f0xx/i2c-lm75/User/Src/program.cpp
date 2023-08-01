#include <stdio.h>
#include <string.h>
#include <math.h>
#include "i2c.h"
#include "qfplib.h"
#include "main.h"

/// Semihosting Initializing.
extern "C" void initialise_monitor_handles( void );

/// Период сигнала, управляющего светодиодом LED_USER, мсек.
#define LED_USER_PERIOD_MSEC    ( 500 )

/// Адрес цифрового датчика температуры (STLM75).
#define I2C_DTS_ADDRESS         ( 0x9E )

/// Адрес регистра.
#define I2C_DTS_TEMP_ADDRESS    ( 0x00 )

/// Количество попыток определения наличия I2C устройства.
#define I2C_IS_DEVICE_READY_TRIALS          (3)

/// Время ожидания ответа от устройства I2C, мсек.
#define I2C_IS_DEVICE_READY_TIMEOUT_MSEC    ( 20 )

/// Признак наличия датчика.
bool SensorAvailable = false;

/// Буфер для значения температуры цифрового датчика (STLM75).
uint8_t SensorData[2] = {0};

/// Значение температуры, 0.1 °C.
int16_t Temp = 0;

/// Измеренное значение.
float SensorValue = 0;

/// Счётчики.
volatile uint32_t TimeTickMs = 0;
uint32_t oldTimeTickHSec = 0;

char buffer[255];

/**
 * \brief   Преобразует сырые данные от датчика.
 *
 */
void UpdateSensorData()
{
    // Крайний левый бит в потоке выходных данных содержит информацию о знаке температуры
    // для каждого преобразования. Если бит знака равен '0', температура положительная,
    // а если бит знака равен '1', температура температура отрицательная.

    // Переворачиваем int16.
    auto temp = ( SensorData[0] << 8 ) + SensorData[1];

    // Знаковый сдвиг вправо (убираем неиспользованные биты).
    temp >>= 7;

    // Преобразуем знаковое целое (см. Temperature data format (STLM75)).
    SensorValue = qfp_int2float( temp );

    // Умножаем (делением) на вес одного разряда, равный 0.5 °C.
    SensorValue = qfp_fdiv_fast( SensorValue, 2 );

    // Значение температуры, 0.1 °C.
    Temp = qfp_float2int( qfp_fmul( 10.0f, SensorValue ) );
}


/**
 * \brief   Выполняет инициализацию.
 *
 */
void init( void )
{
    // Инициализация библиотеки Semihosting.
    initialise_monitor_handles();
}


/**
 * \brief   Выполняет дополнительные настройки.
 *
 */
void setup( void )
{
    // Устанавливаем состояние по умолчанию.
    if ( HAL_GPIO_ReadPin( LED_USER_GPIO_Port, LED_USER_Pin ) == GPIO_PIN_SET )
    {
        HAL_GPIO_WritePin( LED_USER_GPIO_Port, LED_USER_Pin, GPIO_PIN_RESET );
    }

    // Проверяем наличие датчика.
    SensorAvailable = ( HAL_I2C_IsDeviceReady( & hi2c1, I2C_DTS_ADDRESS, I2C_IS_DEVICE_READY_TRIALS, I2C_IS_DEVICE_READY_TIMEOUT_MSEC ) == HAL_OK );

    if ( SensorAvailable )
    {
        // data.csv будет создан на ПК.
        FILE * fd = fopen( "C:\\Data\\data.csv", "w+" );

        if ( fd != NULL )
        {
            // Записываем 10 отсчётов в файл.
            for ( int n = 0; n < 10; n++ )
            {
                // Выполняем синхронное чтение данных, используя DMA.
                HAL_I2C_Master_Receive( & hi2c1, I2C_DTS_ADDRESS, SensorData, sizeof( SensorData ), I2C_IS_DEVICE_READY_TIMEOUT_MSEC );

                HAL_Delay( 500 );

                UpdateSensorData();

                sprintf( buffer, "%i,%.1f\n", n, ( double ) SensorValue );

                // Делаем замену '.' на ','.
                //for ( int i = 0; buffer[i]; i++ ) if ( buffer[i] == '.' ) buffer[i] = ',';

                fwrite( buffer, sizeof( char ), strlen( buffer ), fd );
            }

            fclose( fd );
        }
    }
}


/**
 * \brief   Выполняется периодически в теле основного цикла.
 *
 */
void loop( void )
{
    if ( !SensorAvailable ) return;

    // Выполняем асинхронное чтение данных, используя DMA.
    HAL_I2C_Master_Receive_DMA( & hi2c1, I2C_DTS_ADDRESS, SensorData, sizeof( SensorData ) );

    HAL_Delay( 1000 );

    printf( "Температура: %i.%u °C\n", Temp / 10, ( unsigned ) abs( Temp ) % 10 );
}


/**
 * \brief   Callback-функция периодического таймера SysTick.
 *
 */
void HAL_SYSTICK_Callback( void )
{
    TimeTickMs++;

    if ( TimeTickMs - oldTimeTickHSec > LED_USER_PERIOD_MSEC )
    {
        oldTimeTickHSec = TimeTickMs;

        // Индикация работы основного цикла.
        HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );
    }
}


/**
 * \brief   Callback-функция завершения передачи в режиме чтения.
 *
 * \param   hi2c    указатель на структуру параметров конфигурации типа I2C_HandleTypeDef.
 *
 */
void HAL_I2C_MasterRxCpltCallback( I2C_HandleTypeDef * hi2c )
{
    UpdateSensorData();
}
