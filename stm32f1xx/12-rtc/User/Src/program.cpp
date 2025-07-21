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

    // Инициализация RTC.    
    sTime.Hours     = 0x10; // Час в формате BCD.
    sTime.Minutes   = 0x20; // Минуты в формате BCD.
    sTime.Seconds   = 0x30; // Секунды в формате BCD.

    // Установка времени в RTC.
    if ( HAL_RTC_SetTime( & hrtc, & sTime, RTC_FORMAT_BCD ) != HAL_OK )
    {
        Error_Handler();
    }

    // Установка даты в RTC.
    // Пример: 16 марта 2021 года.
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month   = RTC_MONTH_MARCH;
    sDate.Date    = 0x16;   // День в формате BCD.
    sDate.Year    = 0x21;   // Год в формате BCD (2021 - 2000 = 21).

    // Установка даты в RTC.
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

    // Читаем текущее время и дату из RTC.
    HAL_RTC_GetTime( & hrtc, & sTime, RTC_FORMAT_BIN );
    HAL_RTC_GetDate( & hrtc, & sDate, RTC_FORMAT_BIN );

    // Выводим текущее время и дату в консоль.
    printf( "Date: %02d.%02d.%02d\t", sDate.Date, sDate.Month, sDate.Year );
    printf( "Time: %02d:%02d:%02d\n", sTime.Hours, sTime.Minutes, sTime.Seconds );

    // Читаем состояние порта и выводим его в консоль.
    auto value = HAL_GPIO_ReadPin( LED_USER_GPIO_Port, LED_USER_Pin );

    // Если светодиод включен, то выводим "On", иначе "Off".
    printf( "Led %s\n", value == GPIO_PIN_RESET ? "On" : "Off" );
}


/// @brief  Обработчик прерывания от RTC.
/// @note   Вызывается при каждом событии RTC (например, каждую секунду).
/// @param hrtc     Указатель на структуру RTC_HandleTypeDef, содержащую информацию о RTC.
void HAL_RTCEx_RTCEventCallback( RTC_HandleTypeDef * hrtc )
{
    // Переключаем выход порта (мигаем светодиодом).
    HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );

    PinChanged = true;
}
