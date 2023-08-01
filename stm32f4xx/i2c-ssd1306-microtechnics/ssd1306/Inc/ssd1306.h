/**
  ******************************************************************************
  * @file           : ssd1306.h
  * @brief          : SSD1306 driver header
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef SSD1306_H
#define SSD1306_H


/* Includes ------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

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

#define SSD1306_X_SIZE 128
#define SSD1306_Y_SIZE 64

#define SSD1306_BUFFER_SIZE ( SSD1306_X_SIZE * SSD1306_Y_SIZE ) / 8


/* Functions -----------------------------------------------------------------*/

extern void SSD1306_Init();
extern void SSD1306_ClearScreen();
extern void SSD1306_UpdateScreen();
extern void
SSD1306_DrawFilledRect( uint8_t xStart, uint8_t xEnd, uint8_t yStart, uint8_t yEnd );
extern uint8_t SSD1306_IsReady();

#ifdef __cplusplus
}
#endif

#endif    // #ifndef SSD1306_H
