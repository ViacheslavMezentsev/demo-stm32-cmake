#include <stdio.h>
#include <rtc.h>
#include "main.h"

/// Semihosting Initializing.
extern "C" void initialise_monitor_handles( void );

/// Период сигнала, управляющего светодиодом LED_USER, мсек.
#define LED_USER_PERIOD_MSEC    ( 1000 )

/// Счётчики.
volatile uint32_t TimeTickMs = 0;
uint32_t oldTimeTickHSec = 0;

/// Флаг события.
volatile bool PinChanged = false;

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
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    // Set the Time and Date.
    sTime.Hours          = 0x10;
    sTime.Minutes        = 0x20;
    sTime.Seconds        = 0x30;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    if ( HAL_RTC_SetTime( & hrtc, & sTime, RTC_FORMAT_BCD ) != HAL_OK )
    {
        Error_Handler();
    }

    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month   = RTC_MONTH_MARCH;
    sDate.Date    = 0x16;
    sDate.Year    = 0x21;

    if ( HAL_RTC_SetDate( & hrtc, & sDate, RTC_FORMAT_BCD ) != HAL_OK )
    {
        Error_Handler();
    }
}


/**
 * \brief   Выполняется периодически в теле основного цикла.
 *
 */
void loop( void )
{
    if ( !PinChanged ) return;

    // Сбрасываем флаг.
    PinChanged = false;

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    HAL_RTC_GetTime( & hrtc, & sTime, RTC_FORMAT_BIN );
    HAL_RTC_GetDate( & hrtc, & sDate, RTC_FORMAT_BIN );

    printf( "Date: %02d.%02d.%02d\t", sDate.Date, sDate.Month, sDate.Year );
    printf( "Time: %02d:%02d:%02d\n", sTime.Hours, sTime.Minutes, sTime.Seconds );

    auto value = HAL_GPIO_ReadPin( LED_USER_GPIO_Port, LED_USER_Pin );

    printf( "Led %s\n", value == GPIO_PIN_RESET ? "On" : "Off" );
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

        // Переключаем выход порта (мигаем светодиодом).
        HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );

        PinChanged = true;
    }
}
