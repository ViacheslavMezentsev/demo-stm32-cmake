#include <stm32f4xx_hal.h>

#define LED_Port     GPIOC
#define LED_Pin      GPIO_PIN_13
#define LED_PinState GPIO_PIN_SET

/// Обработчик прерывания SysTick.
extern "C" void SysTick_Handler( void )
{
    HAL_IncTick();
}

/**
 * \brief   Настройка системного тактирования.
 *
 * Настройка тактирования на частоту 16 MHz с использованием внутреннего генератора HSI.
 */
void SystemClock_Config( void )
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    // Configure the main internal regulator output voltage.
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE1 );

    // Настройка источника тактирования (HSI).
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;    // PLL отключен

    if ( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK )
    {
        // Ошибка конфигурации.
        while ( 1 )
        {
        }
    }

    // Настройка системных шин.
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;    // SYSCLK = HSI (16 MHz)
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;         // HCLK = 16 MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;           // APB1 = 16 MHz (max 45 MHz)
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;           // APB2 = 16 MHz (max 90 MHz)

    // Настройка латентности Flash.
    // Для 16 MHz достаточно 0 wait states (диапазон 0-16 MHz при 3.3V)
    if ( HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_0 ) != HAL_OK )
    {
        // Ошибка конфигурации.
        while ( 1 )
        {
        }
    }
}

/**
 * \brief   Выполняет инициализацию портов ввода-вывода.
 *
 */
void initGPIO()
{
    // Включаем тактирование порта.
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Начальный уровень на выходе порта.
    HAL_GPIO_WritePin( LED_Port, LED_Pin, LED_PinState );

    GPIO_InitTypeDef GPIO_Config = {
        .Pin = LED_Pin,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW,
    };

    HAL_GPIO_Init( LED_Port, &GPIO_Config );
}


/**
 * \brief   Точка входа в программу.
 *
 */
int main( void )
{
    // Инициализация библиотеки HAL.
    HAL_Init();

    // Настройка системного тактирования.
    SystemClock_Config();

    // Инициализация портов ввода-вывода.
    initGPIO();

    while ( 1 )
    {
        // Переключаем выход порта.
        HAL_GPIO_TogglePin( LED_Port, LED_Pin );

        HAL_Delay( 500 );
    }
}
