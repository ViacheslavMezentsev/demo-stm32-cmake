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
    LL_GPIO_ResetOutputPin( LED_USER_GPIO_Port, LED_USER_Pin );
}


/**
 * \brief   Выполняется периодически в теле основного цикла.
 *
 */
void loop( void )
{
    // Переключаем выход порта (мигаем светодиодом).
    LL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );

    // Читаем состояние порта и выводим его в консоль.
    auto value = LL_GPIO_IsInputPinSet( LED_USER_GPIO_Port, LED_USER_Pin );

    // Если светодиод включен, то выводим "On", иначе "Off".
    printf( "Led %s\n", value == SET ? "On" : "Off" );

    LL_mDelay( 500 );
}
