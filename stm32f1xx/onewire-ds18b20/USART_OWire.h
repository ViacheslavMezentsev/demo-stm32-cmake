#ifndef USART_OWIRE_H_INCLUDED
#define USART_OWIRE_H_INCLUDED

#include "stm32f10x_conf.h"

#define OW_OK			0
#define OW_ERROR		1
#define OW_NO_DEVICE	2
#define OW_TIMEOUT  	3

#define OW_BUFF_BYTES   16                  // Число байт данных в одной транзакции DMA.
#define OW_TIMEMAX      50000               // Время таймаута ожидания передачи.

void OW_Init(void);
uint8_t OW_Reset(void);
uint8_t OW_CalcCrc8(const uint8_t *Buff, uint8_t Len);
uint8_t OW_WriteArr(const uint8_t *ByteArr, uint16_t Size);
uint8_t OW_ReadArr(uint8_t *ByteArr, uint16_t Size);
uint8_t OW_WriteByte(uint8_t Byte);
uint8_t OW_ReadByte(uint8_t *Byte);
void OW_SearchReset(void);
uint8_t OW_SearchEnumCmd(uint8_t *Buff, uint8_t Cmd);
uint8_t OW_SearchEnum(uint8_t *Buff);

#endif /* USART_OWIRE_H_INCLUDED */
