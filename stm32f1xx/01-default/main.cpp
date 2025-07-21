#include <stm32f1xx_hal.h>
#include "xprintf.h"

/// Определяем макросы для использования xprintf и xputs.
/// Эти макросы позволяют использовать printf и print как синонимы для xprintf и xputs соответственно.
#ifdef XF_USE_OUTPUT
#define printf      xprintf
#define print       xputs
#endif

extern UART_HandleTypeDef huart1;

/// Инициализация USART1.
/// @note    Настройка USART1 производится в файле usart.cpp.
extern void MX_USART1_UART_Init( void );

/// @brief  Функция для передачи одного символа по последовательному порту.
extern "C" void uart_putc( int c );

/// Пин светодиода (STM32F103C8T6).
#define LED_USER_Pin       GPIO_PIN_13
#define LED_USER_GPIO_Port GPIOC

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

    // Вывод значения частоты HCLK в UART.
    printf( "HCLK Frequency: %lu Hz\n", HAL_RCC_GetHCLKFreq() );

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
