/**
  ******************************************************************************
  * @file           : ethernet.c
  * @brief          : Ethernet driver
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */



/* Includes ------------------------------------------------------------------*/

#include <string.h>
#include "ethernet.h"
#include "arp.h"
#include "ip.h"



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
    uint16_t ethDataLen = requestSize - sizeof(ETH_Frame);
    
    // ARP protocol
    if (etherType == ETH_FRAME_TYPE_ARP)
    {
      responseSize = ARP_Process((ARP_Frame*)ethFrame->data, ethDataLen);
    }
    
    // IP protocol
    if (etherType == ETH_FRAME_TYPE_IP)
    {
      responseSize = IP_Process((IP_Frame*)ethFrame->data, ethDataLen);
    }

    if (responseSize > 0)
    {
      ETH_Response(ethFrame, responseSize);
    }
  }
}



/*----------------------------------------------------------------------------*/
