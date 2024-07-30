/**
  ******************************************************************************
  * @file           : icmp.h
  * @brief          : ICMP driver interface
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef ICMP_H
#define ICMP_H



/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"
#include "common.h"



/* Declarations and definitions ----------------------------------------------*/

#define ICMP_FRAME_TYPE_ECHO_REQUEST	                        0x08
#define ICMP_FRAME_TYPE_ECHO_REPLY	                        0x00



typedef struct ICMP_EchoFrame
{
  uint8_t type;
  uint8_t code;
  uint16_t checkSum;
  uint16_t id;
  uint16_t seqNum;
  uint8_t data[];
} ICMP_EchoFrame;



/* Functions -----------------------------------------------------------------*/

extern uint16_t ICMP_Process(ICMP_EchoFrame* icmpFrame, uint16_t frameLen);



#endif // #ifndef ICMP_H
