/**
  ******************************************************************************
  * @file    motion_mc_manager.h
  * @author  MEMS Software Solutions Team
  * @brief   This file contains definitions for the motion_mc_manager.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MOTION_MC_MANAGER_H
#define MOTION_MC_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "motion_mc.h"
#include "main.h"
#include "iks01a2_mems_control.h"

/* Extern variables ----------------------------------------------------------*/
/* Exported Macros -----------------------------------------------------------*/
/* Exported Types ------------------------------------------------------------*/
/* Imported Variables --------------------------------------------------------*/
/* Exported Functions Prototypes ---------------------------------------------*/
void MotionMC_manager_init(int sampletime, unsigned short int enable);
void MotionMC_manager_update(MMC_Input_t *data_in);
void MotionMC_manager_get_params(MMC_Output_t *data_out);
void MotionMC_manager_get_version(char *version, int *length);
void MotionMC_manager_compensate(MOTION_SENSOR_Axes_t *data_raw, MOTION_SENSOR_Axes_t *data_comp);

int32_t mag_val_to_mGauss(float mag_val_uT);

char MotionMC_LoadCalFromNVM(unsigned short int datasize, unsigned int *data);
char MotionMC_SaveCalInNVM(unsigned short int datasize, unsigned int *data);

#ifdef __cplusplus
}
#endif

#endif /* MOTION_MC_MANAGER_H */

