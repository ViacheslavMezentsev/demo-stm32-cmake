#include <stdio.h>
#include <rtc.h>
#include "main.h"

/// Semihosting Initializing.
extern "C" void initialise_monitor_handles( void );

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
    sTime.Hours     = 0x10;
    sTime.Minutes   = 0x20;
    sTime.Seconds   = 0x30;

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

    // Включаем секундное прерывание (настройте свой источник тактирования RTC).
    HAL_RTCEx_SetSecond_IT( & hrtc );
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


void HAL_RTCEx_RTCEventCallback( RTC_HandleTypeDef * hrtc )
{
    // Переключаем выход порта (мигаем светодиодом).
    HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );

    PinChanged = true;
}
