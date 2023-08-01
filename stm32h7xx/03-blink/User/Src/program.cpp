#include <stdio.h>
#include "main.h"

/// Semihosting Initializing.
extern "C" void initialise_monitor_handles( void );

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
}


/**
 * \brief   Выполняется периодически в теле основного цикла.
 *
 */
void loop( void )
{
    // Переключаем выход порта (мигаем светодиодом).
    HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );    

    auto value = HAL_GPIO_ReadPin( LED_USER_GPIO_Port, LED_USER_Pin );

    printf( "Led %s\n", value == GPIO_PIN_RESET ? "On" : "Off" );

    HAL_Delay( 500 );
}
