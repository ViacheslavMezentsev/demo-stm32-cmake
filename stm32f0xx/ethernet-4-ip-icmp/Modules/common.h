/**
  ******************************************************************************
  * @file           : common.h
  * @brief          : Common project data
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef COMMON_H
#define COMMON_H



/* Includes ------------------------------------------------------------------*/



/* Declarations and definitions ----------------------------------------------*/

#define MAC_ADDRESS_BYTES_NUM                                   6
#define IP_ADDRESS_BYTES_NUM                                    4

#define htons(val)                                              ((val << 8) & 0xFF00) | ((val >> 8) & 0xFF)
#define htonl(val)                                              ((val << 8) & 0xFF0000) | ((val >> 8) & 0xFF00) | ((val << 24) & 0xFF000000) | ((val >> 24) & 0xFF)

#define ntohs(val)                                              htons(val)
#define ntohl(val)                                              htonl(val)



extern uint8_t ipAddr[IP_ADDRESS_BYTES_NUM];
extern  uint8_t macAddr[MAC_ADDRESS_BYTES_NUM];



/* Functions -----------------------------------------------------------------*/



#endif // #ifndef COMMON_H
