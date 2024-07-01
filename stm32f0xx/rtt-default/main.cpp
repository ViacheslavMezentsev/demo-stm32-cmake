#include <stm32f0xx_hal.h>
#include "SEGGER_RTT.h"

#define LED_Port        GPIOA
#define LED_Pin         GPIO_PIN_5
#define LED_PinState    GPIO_PIN_SET

// SEGGER RTT: IP: localhost, PORT: 19021.
#define print(s)                        SEGGER_RTT_WriteString( 0, s ); HAL_Delay(1)
#define println(s)                      print( s "\n" )
#define printf( format, ... )           SEGGER_RTT_printf( 0, ( const char * ) ( format ), ##__VA_ARGS__ ); HAL_Delay(1)

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

    // Настройка терминала 0 для работы в неблокирующем режиме.
    SEGGER_RTT_ConfigUpBuffer( 0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM );

    SEGGER_RTT_WriteString( 0, "SEGGER Real-Time-Terminal Sample\n" );

    // Инициализация портов ввода-вывода.
    initGPIO();

    while ( 1 )
    {
        // Переключаем выход порта.
        HAL_GPIO_TogglePin( LED_Port, LED_Pin );

        GPIO_PinState value = HAL_GPIO_ReadPin( LED_Port, LED_Pin );

        printf( "Led %s\n", value == GPIO_PIN_RESET ? "On" : "Off" );

        HAL_Delay( 500 );
    }

    return 0;
}
