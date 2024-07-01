#include "USART_OWire.h"

#define OW_USART 		USART2        // USART к которому подключены 1-Wire устройства.
#define OW_DMA_CH_RX 	DMA1_Channel6 // Канал DMA приема USART.
#define OW_DMA_CH_TX 	DMA1_Channel7 // Канал DMA передачи USART.
#define OW_DMA_FLAG		DMA1_FLAG_TC6 // Флаг канала DMA окончания транзакции.
#define OW_DMA_CLEAR_FLAG (DMA1_FLAG_GL7 | DMA1_FLAG_TC7 | DMA1_FLAG_HT7 | DMA1_FLAG_TE7 | \
                           DMA1_FLAG_TE7 | DMA1_FLAG_GL6 | DMA1_FLAG_TC6 | DMA1_FLAG_HT6 | DMA1_FLAG_TE6)
#define OW_GPIO         GPIOA         // Порт используемый USART.
#define OW_GPIO_PIN     GPIO_Pin_2   // Вывод порта используемый USART.
#define OW_PC_GPIO      RCC_APB2Periph_GPIOA  // Для включения тактирования порта.
#define OW_PC_USART     RCC_APB1Periph_USART2 // Для включения тактирования USART.

#define OW_BUFF_SIZE    (OW_BUFF_BYTES * 8) // Размер буфера DMA. Должен быть кратным 8.

volatile uint8_t OW_Buff[OW_BUFF_SIZE];

uint8_t ROM_NO[8];
int32_t LastDiscrepancy;
int32_t LastFamilyDiscrepancy;
int32_t LastDeviceFlag;


// ******** Приватные функции ********

void Usart_Cnf(uint32_t Rate) // Настройка USART.
{
    USART_InitTypeDef Usart;

    Usart.USART_BaudRate = Rate;
	Usart.USART_WordLength = USART_WordLength_8b;
	Usart.USART_StopBits = USART_StopBits_1;
	Usart.USART_Parity = USART_Parity_No;
	Usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	Usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(OW_USART, &Usart);
}

//-----------------------------------------------------------------------------
// функция преобразует один байт в восемь, для передачи через USART
// ow_byte - байт, который надо преобразовать
// ow_bits - ссылка на буфер, размером не менее 8 байт
//-----------------------------------------------------------------------------
void OW_ToBits(uint8_t ow_byte, uint8_t *ow_bits) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		if (ow_byte & 0x01) {
			*ow_bits = 0xFF;
		} else {
			*ow_bits = 0;
		}
		ow_bits++;
		ow_byte = ow_byte >> 1;
	}
}

//-----------------------------------------------------------------------------
// обратное преобразование - из того, что получено через USART опять собирается байт
// ow_bits - ссылка на буфер, размером не менее 8 байт
//-----------------------------------------------------------------------------
uint8_t OW_ToByte(uint8_t *ow_bits) {
	uint8_t ow_byte, i;
	ow_byte = 0;
	for (i = 0; i < 8; i++) {
		ow_byte = ow_byte >> 1;
		if (*ow_bits == 0xFF) {
			ow_byte |= 0x80;
		}
		ow_bits++;
	}

	return ow_byte;
}

 // Преобразование массива в данные передачи по USART.
uint16_t OW_ArrToBits(const uint8_t *Arr, uint16_t Size)
{
   if ((Arr==0) || (Size==0) || (Size>OW_BUFF_BYTES)) return 0;

   uint16_t i, Pos=0;

   for (i=0; i<Size; i++, Pos+=8)
   {
       OW_ToBits(Arr[i], (uint8_t*) &OW_Buff[Pos]);
   }

   return Pos;
}

// Преобразование данных принятых по USART в массив.
uint16_t OW_ArrToByte(uint8_t *Arr, uint16_t Size)
{
    if ((Arr==0) || (Size==0) || (Size>OW_BUFF_BYTES)) return 0;

    uint16_t i, Pos=0;

    for (i=0; i<Size; i++, Pos+=8)
    {
       Arr[i] = OW_ToByte((uint8_t*) &OW_Buff[Pos]);
    }

    return Pos;
}


uint8_t OW_BitRW(uint8_t *io) // Чтение / запись бита в 1-Wire.
{
    uint8_t Res = OW_OK;

    USART_ClearFlag(OW_USART, USART_FLAG_RXNE | USART_FLAG_TC | USART_FLAG_TXE | USART_FLAG_ORE |
                    USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
	USART_SendData(OW_USART, (uint8_t) *io);

	volatile uint32_t Counter=0;

    while (USART_GetFlagStatus(OW_USART, USART_FLAG_TC) == RESET)
    {
        #ifdef OW_GIVE_TICK_RTOS
           taskYIELD();
        #endif

        if ((Counter++)>=OW_TIMEMAX)
        {
            Res = OW_TIMEOUT;
            break;
        }
    }

    if (Res == OW_OK)
    {
        Counter=0;
        while (USART_GetFlagStatus(OW_USART, USART_FLAG_RXNE) == RESET)
        {
            if ((Counter++)>=OW_TIMEMAX)
            {
                Res = OW_TIMEOUT;
                break;

            }
        }

        if (Res == OW_OK)
        {
           *io = (uint8_t) USART_ReceiveData(OW_USART);
        }
    }

    return Res;
}

uint8_t OW_ReadBit(void) // Чтение бита из 1-Wire.
{
   uint8_t Bit=0xFF;
   OW_BitRW(&Bit);
   return (Bit==0xFF) ? 1 : 0;
}

uint8_t OW_WriteBit(uint8_t Bit) // Запись бита в 1-Wire.
{
   Bit = (Bit) ? 0xFF : 0;
   return OW_BitRW(&Bit);
}

uint8_t OW_StartIO(uint16_t BuffSize) // Запуск DMA транзакции.
{
    uint8_t Res = OW_OK;

    OW_Init();

    DMA_SetCurrDataCounter(OW_DMA_CH_TX, BuffSize); // Для перезапуска DMA.
    DMA_SetCurrDataCounter(OW_DMA_CH_RX, BuffSize);

    USART_ClearFlag(OW_USART, USART_FLAG_RXNE | USART_FLAG_TC | USART_FLAG_TXE | USART_FLAG_ORE |
                    USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE);
    DMA_ClearFlag(OW_DMA_CLEAR_FLAG);
    USART_DMACmd(OW_USART, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(OW_DMA_CH_RX, ENABLE);
    DMA_Cmd(OW_DMA_CH_TX, ENABLE);


    volatile uint32_t Counter=0;

    while (DMA_GetFlagStatus(OW_DMA_FLAG) == RESET) // Ждем завершения DMA транзакции.
    {
        #ifdef OW_GIVE_TICK_RTOS
           taskYIELD();
        #endif

        if ((Counter++)>=OW_TIMEMAX)
        {
            Res = OW_TIMEOUT; // Время вышло - транзакция не завершилась в заданный срок.
            break;
        }
    }

    // отключаем DMA.
    USART_DMACmd(OW_USART, USART_DMAReq_Tx | USART_DMAReq_Rx, DISABLE);
    DMA_Cmd(OW_DMA_CH_TX, DISABLE);
    DMA_Cmd(OW_DMA_CH_RX, DISABLE);

    return Res;
}

uint8_t OW_ByteRW(uint8_t Byte, uint8_t *Ret) // Чтение / запись байта из / в 1-Wire.
{
    uint8_t Res = OW_OK;

    OW_ToBits(Byte, (uint8_t*) OW_Buff);

    if (OW_StartIO(8) == OW_OK)
    {
        if (Ret != 0) *Ret = OW_ToByte((uint8_t*) OW_Buff);
    }
    else Res = OW_TIMEOUT;

    return Res;
}

 // Чтение / запись массива из 1-Wire. Размер массива не должен превышать OW_BUFF_BYTES.
uint8_t OW_ArrRW(const uint8_t *Arr, uint8_t *Ret, uint16_t Size)
{
    uint8_t Res = OW_OK;
    uint16_t BuffSize=0;

    if (((Arr==0) && (Ret==0)) || (Size==0)) return OW_ERROR;

    if (Arr != 0)
    {
        BuffSize = OW_ArrToBits(Arr, Size);
    }
    else
    {
        BuffSize = Size * 8;
        if (BuffSize > OW_BUFF_SIZE) return OW_ERROR;

        uint16_t i;
        for (i=0; i<BuffSize; i++) OW_Buff[i] = 0xFF;
    }

    if (BuffSize==0) return OW_ERROR;

    if (OW_StartIO(BuffSize) == OW_OK)
    {
        if (Ret != 0)
        {
            if (OW_ArrToByte(Ret, Size) != BuffSize) Res = OW_ERROR;
        }
    }
    else Res = OW_TIMEOUT;

    return Res;
}





// ***************** Публичные функции ****************



void OW_Init(void) // Инициализация периферии для 1-Wire.
{
    // Вкл. тактирование.
     RCC_APB2PeriphClockCmd(OW_PC_GPIO | RCC_APB2Periph_AFIO, ENABLE);
     RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
     RCC_APB1PeriphClockCmd(OW_PC_USART, ENABLE);

     // Настройка порта для USART.
     {
         GPIO_InitTypeDef Gpio;
         Gpio.GPIO_Pin = OW_GPIO_PIN;
         Gpio.GPIO_Mode = GPIO_Mode_AF_OD;
         Gpio.GPIO_Speed = GPIO_Speed_2MHz;

         GPIO_Init(OW_GPIO, &Gpio);

         Gpio.GPIO_Pin = GPIO_Pin_3;
         Gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;

         GPIO_Init(OW_GPIO, &Gpio);
     }

    Usart_Cnf(115200);                       // Настройка USART.
   // USART_SetAddress(OW_USART, 0);
   // USART_HalfDuplexCmd(OW_USART , ENABLE); // Разрешение работы USART в полудуплексном режиме.

    USART_Cmd(OW_USART, ENABLE);            // Включаем USART.

    // Настройка DMA.
   {
        DMA_InitTypeDef Dma;

        DMA_DeInit(OW_DMA_CH_RX);
        DMA_DeInit(OW_DMA_CH_TX);

        Dma.DMA_PeripheralBaseAddr = (uint32_t) &(OW_USART->DR);
        Dma.DMA_MemoryBaseAddr = (uint32_t) OW_Buff;
        Dma.DMA_DIR = DMA_DIR_PeripheralSRC;
        Dma.DMA_BufferSize = 0;
        Dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
        Dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
        Dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
        Dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
        Dma.DMA_Mode = DMA_Mode_Normal;
        Dma.DMA_Priority = DMA_Priority_Low;
        Dma.DMA_M2M = DMA_M2M_Disable;
        DMA_Init(OW_DMA_CH_RX, &Dma); // Настройка канала приема.

        Dma.DMA_DIR = DMA_DIR_PeripheralDST;
        DMA_Init(OW_DMA_CH_TX, &Dma); // Настройка канала передачи.

   }

}


uint8_t OW_ReadByte(uint8_t *Byte) // Чтение байта из 1-Wire.
{
    return OW_ByteRW(0xFF, Byte);
}

uint8_t OW_WriteByte(uint8_t Byte) // Запись байта в 1-Wire.
{
    return OW_ByteRW(Byte, 0);
}

uint8_t OW_ReadArr(uint8_t *ByteArr, uint16_t Size) // Чтение массива из 1-Wire.
{
    if ((ByteArr==0)||(Size==0)) return OW_ERROR;

    uint8_t Res = OW_OK, Temp;
    uint16_t i;

    for (i=0; i<Size; i+=OW_BUFF_BYTES)
    {
        Temp = OW_ArrRW(0, &ByteArr[i], ((Size-i)>=OW_BUFF_BYTES) ? OW_BUFF_BYTES : (Size-i) );
        if (Temp != OW_OK)
        {
            Res = Temp;
            break;
        }
    }

    return Res;
}

uint8_t OW_WriteArr(const uint8_t *ByteArr, uint16_t Size) // Запись массива в 1-Wire.
{
    if ((ByteArr==0)||(Size==0)) return OW_ERROR;

    uint8_t Res = OW_OK, Temp;
    uint16_t i;

    for (i=0; i<Size; i+=OW_BUFF_BYTES)
    {
        Temp = OW_ArrRW(&ByteArr[i], 0, ((Size-i)>=OW_BUFF_BYTES) ? OW_BUFF_BYTES : (Size-i) );
        if (Temp != OW_OK)
        {
            Res = Temp;
            break;
        }
    }

    return Res;
}


uint8_t OW_Reset(void) // Сброс 1-Wire.
{
    uint8_t Res;

    Usart_Cnf(9600);

    uint8_t x=0xF0;
    Res = OW_BitRW(&x);

    if (Res == OW_OK)
    {
       if (x == 0xF0) Res = OW_NO_DEVICE;
       else if ((x == 0x00) || (x == 0xFF)) Res = OW_ERROR;
    }

    Usart_Cnf(115200);
    return Res;
}

// Подсчет CRC8 массива Buff длиной Len
uint8_t OW_CalcCrc8(const uint8_t *Buff, uint8_t Len)
{
    uint8_t i,dat,crc,fb,st_byt;
    st_byt=0;
    crc=0;
    do
    {
        dat=Buff[st_byt];
        for( i=0; i<8; i++)    // счетчик битов в байте
        {
            fb = crc ^ dat;
            fb &= 1;
            crc >>= 1;
            dat >>= 1;
            if( fb == 1 ) crc ^= 0x8c; // полином
        }
        st_byt++;
    }
    while( st_byt < Len );   // счетчик байтов в массиве
    return crc;
}

void OW_SearchReset(void) // Сброс поиска устройств.
{
   LastDiscrepancy = 0;
   LastDeviceFlag = 0;
   LastFamilyDiscrepancy = 0;
}


// https://www.maximintegrated.com/en/app-notes/index.mvp/id/187
uint8_t OW_SearchEnumCmd(uint8_t *Buff, uint8_t Cmd) // Перечисление устройств.
{
   int32_t id_bit_number;
   int32_t last_zero, rom_byte_number;
   int32_t id_bit, cmp_id_bit;
   uint8_t rom_byte_mask, search_direction, search_result;

   // initialize for search
   id_bit_number = 1;
   last_zero = 0;
   rom_byte_number = 0;
   rom_byte_mask = 1;
   search_result = 0;

   // if the last call was not the last one
   if (!LastDeviceFlag)
   {
      // 1-Wire reset
      if (OW_Reset() != OW_OK)
      {
         // reset the search
         OW_SearchReset();
         return 0;
      }

      // issue the search command
      if (OW_WriteByte(Cmd) != OW_OK)
      {
         OW_SearchReset();
         return 0;
      }

      // loop to do the search
      do
      {
         // read a bit and its complement
         id_bit = OW_ReadBit();
         cmp_id_bit = OW_ReadBit();

         // check for no devices on 1-wire
         if ((id_bit == 1) && (cmp_id_bit == 1))
            break;
         else
         {
            // all devices coupled have 0 or 1
            if (id_bit != cmp_id_bit)
               search_direction = id_bit;  // bit write value for search
            else
            {
               // if this discrepancy if before the Last Discrepancy
               // on a previous next then pick the same as last time
               if (id_bit_number < LastDiscrepancy)
                  search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
               else
                  // if equal to last pick 1, if not then pick 0
                  search_direction = (id_bit_number == LastDiscrepancy);

               // if 0 was picked then record its position in LastZero
               if (search_direction == 0)
               {
                  last_zero = id_bit_number;

                  // check for Last discrepancy in family
                  if (last_zero < 9)
                     LastFamilyDiscrepancy = last_zero;
               }
            }

            // set or clear the bit in the ROM byte rom_byte_number
            // with mask rom_byte_mask
            if (search_direction == 1)
              ROM_NO[rom_byte_number] |= rom_byte_mask;
            else
              ROM_NO[rom_byte_number] &= ~rom_byte_mask;

            // serial number search direction write bit
            OW_WriteBit(search_direction);

            // increment the byte counter id_bit_number
            // and shift the mask rom_byte_mask
            id_bit_number++;
            rom_byte_mask <<= 1;

            // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
            if (rom_byte_mask == 0)
            {
                rom_byte_number++;
                rom_byte_mask = 1;
            }
         }
      }
      while(rom_byte_number < 8);  // loop until through all ROM bytes 0-7

      // if the search was successful then
      if (!((id_bit_number < 65) ))
      {
         // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
         LastDiscrepancy = last_zero;

         // check for last device
         if (LastDiscrepancy == 0)
            LastDeviceFlag = 1;

         search_result = 1;
      }
   }

   // if no device found then reset counters so next 'search' will be like a first
   if (!search_result || !ROM_NO[0])
   {
      OW_SearchReset();
      search_result = 0;

   }
   else if (Buff != 0)
   {
       uint8_t i;
       for (i=0; i<8; i++) Buff[i] = ROM_NO[i];
   }

   return search_result;

}

uint8_t OW_SearchEnum(uint8_t *Buff) // Перечисление устройств.
{
   return OW_SearchEnumCmd(Buff, 0xF0);
}
