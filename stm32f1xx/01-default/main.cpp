#include "main.h"

const char Text1[] = "Привет из STM32! (def)\n";

/// Обработчик прерывания SysTick.
/// @note    SysTick используется для реализации функции HAL_Delay().
extern "C" void SysTick_Handler( void )
{
    HAL_IncTick();
}


/**
 * \brief   Инициализация портов ввода-вывода.
 *
 */
void initGPIO()
{
    // Включаем тактирование.
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Начальный уровень на выходе порта.
    HAL_GPIO_WritePin( LED_USER_GPIO_Port, LED_USER_Pin, GPIO_PIN_SET );

    GPIO_InitTypeDef GPIO_Config = {
        .Pin = LED_USER_Pin,
        .Mode = GPIO_MODE_OUTPUT_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW,
    };

    HAL_GPIO_Init( LED_USER_GPIO_Port, &GPIO_Config );
}


/**
 * \brief   Точка входа в программу.
 *
 */
int main()
{
    // Инициализация библиотеки HAL.
    HAL_Init();

    // Инициализация портов ввода-вывода.
    initGPIO();

    // Настройка USART1.
    MX_USART1_UART_Init();

    // Устанавливаем функцию вывода.
    xdev_out( uart_putc );

    println( "\n--- System Clock Information (using HAL) ---" );

    print( "Target CPU Frequency (HSE_VALUE): " );
    printf( "%lu", HSE_VALUE / 1000000 );
    println( " MHz" );

    // Получаем "реальные" текущие частоты из регистров контроллера с помощью HAL-функций.
    print( "Actual SYSCLK Frequency:      " );
    printf( "%lu", HAL_RCC_GetSysClockFreq() / 1000000 );
    println( " MHz" );

    print( "HCLK (Core, AHB) Frequency:   " );
    printf( "%lu", HAL_RCC_GetHCLKFreq() / 1000000 );
    println( " MHz" );

    print( "PCLK1 (APB1) Frequency:       " );
    printf( "%lu", HAL_RCC_GetPCLK1Freq() / 1000000 );
    println( " MHz  <-- UART2, UART3 are here" );

    print( "PCLK2 (APB2) Frequency:       " );
    printf( "%lu", HAL_RCC_GetPCLK2Freq() / 1000000 );
    println( " MHz  <-- UART1 is here" );

    println( "------------------------------------------" );

    while ( 1 )
    {
        // Переключаем выход порта (мигаем светодиодом).
        HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );

        // Задержка 500 мс.
        HAL_Delay( 500 );

        // Отправляем строку в UART.
        print( Text1 );
    }
}
