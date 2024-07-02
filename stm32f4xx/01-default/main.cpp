#include <stm32f4xx_hal.h>

#define LED_Port        GPIOA
#define LED_Pin         GPIO_PIN_1
#define LED_PinState    GPIO_PIN_SET

/// Обработчик прерывания SysTick.
extern "C" void SysTick_Handler( void )
{
    HAL_IncTick();
}


/**
 * \brief   Выполняет инициализацию портов ввода-вывода.
 *
 */
void initGPIO()
{
    // Включаем тактирование порта.
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Начальный уровень на выходе порта.
    HAL_GPIO_WritePin( LED_Port, LED_Pin, LED_PinState );

    GPIO_InitTypeDef GPIO_Config =
    {
        .Pin   = LED_Pin,
        .Mode  = GPIO_MODE_OUTPUT_PP,
        .Pull  = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW,
    };

    HAL_GPIO_Init( LED_Port, & GPIO_Config );
}


/**
 * \brief   Точка входа в программу.
 *
 */
int main( void )
{
    // Инициализация библиотеки HAL.
    HAL_Init();

    // Инициализация портов ввода-вывода.
    initGPIO();

    while ( 1 )
    {
        // Переключаем выход порта.
        HAL_GPIO_TogglePin( LED_Port, LED_Pin );

        HAL_Delay( 500 );
    }

    return 0;
}
