/**
  ******************************************************************************
  * @file           : arp.c
  * @brief          : ARP driver
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */



/* Includes ------------------------------------------------------------------*/

#include "arp.h"
#include <string.h>



/* Declarations and definitions ----------------------------------------------*/




/* Functions -----------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
uint16_t ARP_Process(ARP_Frame* arpFrame, uint16_t frameLen)
{
  uint16_t newFrameLen = 0;
  
  if (memcmp(arpFrame->destIpAddr, ipAddr, IP_ADDRESS_BYTES_NUM) == 0)
  {
    if (arpFrame->opCode == ntohs(ARP_OP_CODE_REQUEST))
    {
      memcpy(arpFrame->destMacAddr, arpFrame->srcMacAddr, MAC_ADDRESS_BYTES_NUM);
      memcpy(arpFrame->srcMacAddr, macAddr, MAC_ADDRESS_BYTES_NUM);
      
      memcpy(arpFrame->destIpAddr, arpFrame->srcIpAddr, IP_ADDRESS_BYTES_NUM);
      memcpy(arpFrame->srcIpAddr, ipAddr, IP_ADDRESS_BYTES_NUM);
      
      arpFrame->opCode = htons(ARP_OP_CODE_RESPONSE);
      newFrameLen = frameLen;
    }
  }
  
  return newFrameLen;
}



/*----------------------------------------------------------------------------*/
