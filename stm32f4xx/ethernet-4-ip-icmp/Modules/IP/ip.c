/**
  ******************************************************************************
  * @file           : ip.c
  * @brief          : IP driver
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */



/* Includes ------------------------------------------------------------------*/

#include "ip.h"
#include "icmp.h"
#include "ethernet.h"
#include <string.h>



/* Declarations and definitions ----------------------------------------------*/




/* Functions -----------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
uint16_t IP_CalcCheckSum(uint8_t* data, uint16_t len)
{
  uint32_t res = 0;
  uint16_t* ptr = (uint16_t*)data;

  while (len > 1) 
  {
    res += *ptr;
    ptr++;
    len -= 2;
  }
  
  if (len > 0) 
  {
    res += *(uint8_t*)ptr;
  }
  
  while (res > 0xffff)
  {
    res = (res >> 16) + (res & 0xFFFF);
  }
  
  return ~((uint16_t)res);
}



/*----------------------------------------------------------------------------*/
uint16_t IP_Process(IP_Frame* ipFrame, uint16_t frameLen)
{
  uint16_t newFrameLen = 0;

  if (memcmp(ipFrame->destIpAddr, ipAddr, IP_ADDRESS_BYTES_NUM) == 0)
  {             
    uint16_t rxCheckSum = ipFrame->checkSum;
    ipFrame->checkSum = 0;
    uint16_t calcCheckSum = IP_CalcCheckSum((uint8_t*)ipFrame, sizeof(IP_Frame));
    
    if (rxCheckSum == calcCheckSum)
    {   
      uint16_t dataLen = frameLen - sizeof(IP_Frame);
      uint16_t newDataLen = 0;
        
      if (ipFrame->protocol == IP_FRAME_PROTOCOL_ICMP)
      {
        newDataLen = ICMP_Process((ICMP_EchoFrame*)ipFrame->data, dataLen);
      }
      
      newFrameLen = newDataLen + sizeof(IP_Frame);
      ipFrame->len = htons(newFrameLen);
      
      ipFrame->fragId = 0;
      ipFrame->fragOffset = 0;
      
      memcpy(ipFrame->destIpAddr, ipFrame->srcIpAddr, IP_ADDRESS_BYTES_NUM);
      memcpy(ipFrame->srcIpAddr, ipAddr, IP_ADDRESS_BYTES_NUM);
      
      ipFrame->checkSum = IP_CalcCheckSum((uint8_t*)ipFrame, sizeof(IP_Frame));
    }
  }
  
  return newFrameLen;
}



/*----------------------------------------------------------------------------*/
