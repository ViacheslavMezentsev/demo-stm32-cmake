/**
  ******************************************************************************
  * @file           : icmp.c
  * @brief          : ICMP driver
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */



/* Includes ------------------------------------------------------------------*/

#include "icmp.h"
#include "ip.h"



/* Declarations and definitions ----------------------------------------------*/



/* Functions -----------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
uint16_t ICMP_Process(ICMP_EchoFrame* icmpFrame, uint16_t frameLen)
{
  uint16_t newFrameLen = 0;
  
  uint16_t rxCheckSum = icmpFrame->checkSum;
  icmpFrame->checkSum = 0;
  uint16_t calcCheckSum = IP_CalcCheckSum((uint8_t*)icmpFrame, frameLen);
  
  if (rxCheckSum == calcCheckSum)
  {
    if (icmpFrame->type == ICMP_FRAME_TYPE_ECHO_REQUEST)
    {
      icmpFrame->type = ICMP_FRAME_TYPE_ECHO_REPLY;
      icmpFrame->checkSum = IP_CalcCheckSum((uint8_t*)icmpFrame, frameLen);
      newFrameLen = frameLen;
    }
  }
  
  return newFrameLen;
}



/*----------------------------------------------------------------------------*/
