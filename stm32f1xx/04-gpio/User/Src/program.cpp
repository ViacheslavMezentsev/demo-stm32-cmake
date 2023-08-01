#include "main.h"

/// Период сигнала, управляющего светодиодом LED_USER, мсек.
#define LED_USER_PERIOD_MSEC    ( 500 )

/// Счётчики.
volatile uint32_t TimeTickMs = 0;
uint32_t oldTimeTickHSec = 0;

/**
 * \brief   Выполняет инициализацию.
 *
 */
void init( void )
{
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
}


/**
 * \brief   Выполняется периодически в теле основного цикла.
 *
 */
void loop( void )
{
    if ( TimeTickMs - oldTimeTickHSec > LED_USER_PERIOD_MSEC )
    {
        oldTimeTickHSec = TimeTickMs;

        // Индикация работы основного цикла.
        HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );
    }
}


/**
 * \brief   Callback-функция периодического таймера SysTick.
 *
 */
void HAL_SYSTICK_Callback( void )
{
    TimeTickMs++;
}
