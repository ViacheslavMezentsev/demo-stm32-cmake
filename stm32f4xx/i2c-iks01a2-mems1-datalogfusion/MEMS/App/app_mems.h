/**
  ******************************************************************************
  * File Name          : app_mems.h
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-MEMS1.9.6.0 instances.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_MEMS_H
#define __APP_MEMS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void MX_MEMS_Init(void);
void MX_MEMS_Process(void);

void MotionMC_manager_MagCal_start(int sampletime);
void MotionMC_manager_MagCal_stop(int sampletime);

#ifdef __cplusplus
}
#endif

#endif /* __APP_MEMS_H */
