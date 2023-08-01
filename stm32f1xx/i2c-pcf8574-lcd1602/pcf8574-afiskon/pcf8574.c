#include <string.h>
#include "i2c.h"
#include "main.h"
#include "pcf8574.h"

#define PIN_RS    ( 1 << 0 )
#define PIN_EN    ( 1 << 2 )
#define BACKLIGHT ( 1 << 3 )

#define LCD_DELAY_MS    (5)

void I2C_Scan( void )
{
    HAL_StatusTypeDef res;

    for ( uint16_t i = 0; i < 128; i++ )
    {
        res = HAL_I2C_IsDeviceReady( & hi2c1, i << 1, 1, 10 );
    }
}


HAL_StatusTypeDef LCD_SendInternal( uint8_t lcd_addr, uint8_t data, uint8_t flags )
{
    HAL_StatusTypeDef res;

    for ( ;; )
    {
        res = HAL_I2C_IsDeviceReady( & hi2c1, lcd_addr, 1, HAL_MAX_DELAY );

        if ( res == HAL_OK ) break;
    }

    uint8_t up = data & 0xF0;
    uint8_t lo = ( data << 4 ) & 0xF0;

    uint8_t data_arr[4];

    data_arr[0] = up | flags | BACKLIGHT | PIN_EN;
    data_arr[1] = up | flags | BACKLIGHT;
    data_arr[2] = lo | flags | BACKLIGHT | PIN_EN;
    data_arr[3] = lo | flags | BACKLIGHT;

    res = HAL_I2C_Master_Transmit( & hi2c1, lcd_addr, data_arr, sizeof( data_arr ), HAL_MAX_DELAY );
    
    HAL_Delay( LCD_DELAY_MS );

    return res;
}


void LCD_SendCommand( uint8_t lcd_addr, uint8_t cmd )
{
    LCD_SendInternal( lcd_addr, cmd, 0 );
}


void LCD_SendData( uint8_t lcd_addr, uint8_t data )
{
    LCD_SendInternal( lcd_addr, data, PIN_RS );
}


void LCD_Init( uint8_t lcd_addr )
{
    // 4-bit mode, 2 lines, 5x7 format
    LCD_SendCommand( lcd_addr, 0b00110000 );

    // display & cursor home (keep this!)
    LCD_SendCommand( lcd_addr, 0b00000010 );

    // display on, right shift, underline off, blink off
    LCD_SendCommand( lcd_addr, 0b00001100 );

    // clear display (optional here)
    LCD_SendCommand( lcd_addr, 0b00000001 );
}


void LCD_SendString( uint8_t lcd_addr, char* str )
{
    while ( * str )
    {
        LCD_SendData( lcd_addr, ( uint8_t ) ( *str ) );

        str++;
    }
}
