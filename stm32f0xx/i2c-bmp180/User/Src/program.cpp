#include <stdio.h>
#include <string.h>
#include "i2c.h"
#include "bmp180_for_stm32_hal.h"
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
	BMP180_Init( & hi2c1 );

	BMP180_SetOversampling( BMP180_STANDARD );

	// Update calibration data. Must be called once before entering main loop.
	BMP180_UpdateCalibrationData();
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
    // Считать температуру.
    int32_t temperature = BMP180_GetRawTemperature();

    printf( "%d.%d\n", ( int ) temperature / 10, ( int ) temperature % 10 );

    // Считать давление.
    int32_t pressure = BMP180_GetPressure();

    printf( "%d\n", ( int ) pressure );

    HAL_Delay( 1000 );
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

        // Индикация работы основного цикла.
        HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );
    }
}
