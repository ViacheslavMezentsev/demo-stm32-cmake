/**
  ******************************************************************************
  * @file           : ssd1306_interface.h
  * @brief          : SSD1306 driver interface header
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef SSD1306_INTERFACE_H
#define SSD1306_INTERFACE_H


/* Includes ------------------------------------------------------------------*/

#include "stm32f1xx_hal.h"


/* Declarations and definitions ----------------------------------------------*/

#define SSD1306_I2C_TIMEOUT              100
#define SSD1306_I2C_ADDRESS              0x78
#define SSD1306_I2C_CONTROL_BYTE_COMMAND 0x00
#define SSD1306_I2C_CONTROL_BYTE_DATA    0x40


typedef enum
{
    SSD1306_READY = 0x00,
    SSD1306_BUSY  = 0x01
} SSD1306_State;


/* Functions -----------------------------------------------------------------*/

extern void SendCommand( uint8_t* data, uint8_t size );
extern void SendData( uint8_t* data, uint16_t size );


#endif    // #ifndef SSD1306_INTERFACE_H
