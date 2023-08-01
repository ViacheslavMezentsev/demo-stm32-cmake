#include "ssd1306.h"
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
    // Дисплей на базе контроллера SSD1306. Библиотека для STM32.
    // https://microtechnics.ru/displej-na-baze-kontrollera-ssd1306-biblioteka-dlya-stm32/
    
    // Инициализация SSD1306.
    SSD1306_Init();

    // Очистка дисплея.
    SSD1306_ClearScreen();
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
    // Выполняем набор тестов.
    for ( uint8_t  i = 0; i < 8; i++ )
    {
        SSD1306_DrawFilledRect( i * 16, i * 16 + 8, 16, 48 );

        // Обновление дисплея.
        SSD1306_UpdateScreen();

        while ( SSD1306_IsReady() == HAL_OK );
      
        HAL_Delay( 25 );
    }
    
    // Очистка дисплея.
    SSD1306_ClearScreen();

    while( SSD1306_IsReady() == HAL_OK );
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
