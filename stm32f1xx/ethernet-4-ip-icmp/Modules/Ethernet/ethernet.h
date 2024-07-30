/**
  ******************************************************************************
  * @file           : ethernet.h
  * @brief          : Ethernet driver interface
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef ETHERNET_H
#define ETHERNET_H



/* Includes ------------------------------------------------------------------*/

#include "stm32f1xx_hal.h"
#include "enc28j60.h"
#include "common.h"



/* Declarations and definitions ----------------------------------------------*/

#define ETH_FRAME_TYPE_ARP                                      0x0806
#define ETH_FRAME_TYPE_IP                                       0x0800



typedef struct ETH_Frame 
{
  uint8_t destMacAddr[MAC_ADDRESS_BYTES_NUM];
  uint8_t srcMacAddr[MAC_ADDRESS_BYTES_NUM];
  uint16_t etherType;
  uint8_t data[];
} ETH_Frame;



/* Functions -----------------------------------------------------------------*/

void ETH_Process(ENC28J60_Frame* encFrame);



#endif // #ifndef ETHERNET_H
