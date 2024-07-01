#ifndef _PCF8574_AFISKON_H_
#define _PCF8574_AFISKON_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t I2C_Scan( void );
void LCD_Init( uint8_t lcd_addr );
void LCD_SendCommand( uint8_t lcd_addr, uint8_t cmd );
void LCD_SendData( uint8_t lcd_addr, uint8_t data );
void LCD_SendString( uint8_t lcd_addr, char* str );

#ifdef __cplusplus
}
#endif

#endif
