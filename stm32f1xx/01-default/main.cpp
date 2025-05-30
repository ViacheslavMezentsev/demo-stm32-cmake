#include <stm32f1xx_hal.h>
#include "xprintf.h"

extern void MX_USART1_UART_Init( void );

/// Обработчик прерывания SysTick.
extern "C" void SysTick_Handler( void )
{
    HAL_IncTick();
}

const char Text1[] = "Привет из STM32! (def)\n";

#define LED_USER_Pin       GPIO_PIN_13
#define LED_USER_GPIO_Port GPIOC

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

    // Настройка UART.
    MX_USART1_UART_Init();

    while ( 1 )
    {
        // Переключаем выход порта (мигаем светодиодом).
        HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );

        HAL_Delay( 500 );

        xputs( Text1 );
    }
}
