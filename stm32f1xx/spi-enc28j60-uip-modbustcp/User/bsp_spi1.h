#ifndef __BSP_SPI1_H
#define __BSP_SPI1_H

#include "stm32f10x.h"

void BSP_ConfigSPI1( void );
uint8_t BSP_SPI1SendByte( uint8_t byte );
uint8_t BSP_SPI1ReceiveByte( void );

#endif
