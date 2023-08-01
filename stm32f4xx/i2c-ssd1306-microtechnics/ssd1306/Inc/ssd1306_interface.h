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

#if defined(STM32F0)
#include "stm32f0xx_hal.h"
#elif defined(STM32F1)
#include "stm32f1xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#elif defined(STM32L0)
#include "stm32l0xx_hal.h"
#elif defined(STM32L1)
#include "stm32l1xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#elif defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#elif defined(STM32G0)
#include "stm32g0xx_hal.h"
#elif defined(STM32G4)
#include "stm32g4xx_hal.h"
#else
#error "SSD1306 library was tested only on STM32F0, STM32F1, STM32F3, STM32F4, STM32F7, STM32L0, STM32L1, STM32L4, STM32H7, STM32G0, STM32G4 MCU families. Please modify ssd1306.h if you know what you are doing. Also please send a pull request if it turns out the library works on other MCU's as well!"
#endif


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
