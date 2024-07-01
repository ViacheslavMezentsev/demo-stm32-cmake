#include "i2c.h"
#include "pcf8574.h"
#include "main.h"

/// Период сигнала, управляющего светодиодом LED_USER, мсек.
#define LED_USER_PERIOD_MSEC    ( 500 )

#define LCD_ADDR ( 0x27 << 1 )

/// Счётчики.
volatile uint32_t TimeTickMs = 0;
uint32_t oldTimeTickHSec = 0;

#ifdef _PCF8574_AFISKON_H_

/**
 * \brief   Выполняет инициализацию дисплея.
 *
 */
void LCD_Initialize()
{
    I2C_Scan();

    LCD_Init( LCD_ADDR );
}

#endif

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
#ifdef _PCF8574_AFISKON_H_

    // Инициализируем дисплей.
    LCD_Initialize();

    // Установить адрес 0x00.
    LCD_SendCommand( LCD_ADDR, 0b10000000 );
    LCD_SendString( LCD_ADDR, ( char * ) " Using 1602 LCD" );

    // Установить адрес 0x40.
    LCD_SendCommand( LCD_ADDR, 0b11000000 );
    LCD_SendString( LCD_ADDR, ( char * ) "  over I2C bus" );

#endif

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
