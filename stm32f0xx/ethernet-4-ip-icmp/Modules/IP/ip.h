/**
  ******************************************************************************
  * @file           : ip.h
  * @brief          : IP driver interface
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef IP_H
#define IP_H



/* Includes ------------------------------------------------------------------*/

#include "stm32f0xx_hal.h"
#include "common.h"



/* Declarations and definitions ----------------------------------------------*/

#define IP_FRAME_PROTOCOL_ICMP                                  0x01



typedef struct IP_Frame
{
  uint8_t verHeaderLen;
  uint8_t diffServices;
  uint16_t len;
  uint16_t fragId;
  uint16_t fragOffset;
  uint8_t timeToLive;
  uint8_t protocol;
  uint16_t checkSum;
  uint8_t srcIpAddr[IP_ADDRESS_BYTES_NUM];
  uint8_t destIpAddr[IP_ADDRESS_BYTES_NUM];
  uint8_t data[];
} IP_Frame;



/* Functions -----------------------------------------------------------------*/

extern uint16_t IP_CalcCheckSum(uint8_t* data, uint16_t len);
extern uint16_t IP_Process(IP_Frame* ipFrame, uint16_t frameLen);



#endif // #ifndef IP_H
