/**
  ******************************************************************************
  * @file           : ethernet.c
  * @brief          : Ethernet driver
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */



/* Includes ------------------------------------------------------------------*/

#include "ethernet.h"
#include "arp.h"
#include <string.h>



/* Declarations and definitions ----------------------------------------------*/



/* Functions -----------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
static void ETH_Response(ETH_Frame* ethFrame, uint16_t len)
{
  memcpy(ethFrame->destMacAddr, ethFrame->srcMacAddr, MAC_ADDRESS_BYTES_NUM);
  memcpy(ethFrame->srcMacAddr, macAddr, MAC_ADDRESS_BYTES_NUM);
  
  ENC28J60_TransmitFrame((uint8_t*)ethFrame, len + sizeof(ETH_Frame));
}



/*----------------------------------------------------------------------------*/
void ETH_Process(ENC28J60_Frame* encFrame)
{
  uint16_t responseSize = 0;
  uint16_t requestSize = ENC28J60_ReceiveFrame(encFrame);

  if (requestSize > 0)
  {
    ETH_Frame* ethFrame = (ETH_Frame*)encFrame->data;
    uint16_t etherType = ntohs(ethFrame->etherType);
    
    // ARP protocol
    if (etherType == ETH_FRAME_TYPE_ARP)
    {
      uint16_t arpFrameLen = requestSize - sizeof(ETH_Frame);
      responseSize = ARP_Process((ARP_Frame*)ethFrame->data, arpFrameLen);
    }

    if (responseSize > 0)
    {
      ETH_Response(ethFrame, responseSize);
    }
  }
}



/*----------------------------------------------------------------------------*/
