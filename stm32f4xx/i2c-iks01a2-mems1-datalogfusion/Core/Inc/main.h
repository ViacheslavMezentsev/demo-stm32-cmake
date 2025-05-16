/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "stm32f4xx_nucleo.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
void init( void );
void publish( void );
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define ALGO_FREQ  100U /* Algorithm frequency 100Hz */
#define ACC_ODR  ((float)ALGO_FREQ)
#define ACC_FS  4 /* FS = <-4g, 4g> */
#define ALGO_PERIOD  (1000U / ALGO_FREQ) /* Algorithm period [ms] */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_TIM3_Init(void);

/* USER CODE BEGIN EFP */
void DisplayNumber( uint16_t number );
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BSP_BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define S1_Pin GPIO_PIN_1
#define S1_GPIO_Port GPIOA
#define S2_Pin GPIO_PIN_4
#define S2_GPIO_Port GPIOA
#define LED_D2_Pin GPIO_PIN_6
#define LED_D2_GPIO_Port GPIOA
#define S3_Pin GPIO_PIN_0
#define S3_GPIO_Port GPIOB
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SPI3_STB_Pin GPIO_PIN_11
#define SPI3_STB_GPIO_Port GPIOC
#define SPI3_DIO_Pin GPIO_PIN_12
#define SPI3_DIO_GPIO_Port GPIOC
#define LATCH_DIO_Pin GPIO_PIN_5
#define LATCH_DIO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
