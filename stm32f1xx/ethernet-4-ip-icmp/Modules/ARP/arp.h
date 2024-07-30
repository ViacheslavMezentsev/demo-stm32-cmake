/**
  ******************************************************************************
  * @file           : arp.h
  * @brief          : ARP driver interface
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef ARP_H
#define ARP_H



/* Includes ------------------------------------------------------------------*/

#include "stm32f1xx_hal.h"
#include "common.h"



/* Declarations and definitions ----------------------------------------------*/

#define ARP_OP_CODE_REQUEST                                     0x0001
#define ARP_OP_CODE_RESPONSE                                    0x0002



typedef struct ARP_Frame
{
  uint16_t hType;
  uint16_t pType;
  uint8_t hLen;
  uint8_t pLen;
  uint16_t opCode;
  uint8_t srcMacAddr[MAC_ADDRESS_BYTES_NUM];
  uint8_t srcIpAddr[IP_ADDRESS_BYTES_NUM];
  uint8_t destMacAddr[MAC_ADDRESS_BYTES_NUM];
  uint8_t destIpAddr[IP_ADDRESS_BYTES_NUM];
} ARP_Frame;



/* Functions -----------------------------------------------------------------*/

extern uint16_t ARP_Process(ARP_Frame* arpFrame, uint16_t frameLen);



#endif // #ifndef ARP_H
