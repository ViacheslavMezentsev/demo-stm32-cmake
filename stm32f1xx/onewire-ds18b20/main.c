/*
**
**                           Main.c
**
**
**********************************************************************/
/*
   Last committed:     $Revision: 00 $
   Last changed by:    $Author: $
   Last changed date:  $Date:  $
   ID:                 $Id:  $

**********************************************************************/
#include "stm32f10x_conf.h"
#include "USART_OWire.h"
#include "SysTimer.h"

extern uint32_t SystemCoreClock;
volatile uint32_t DmaBuff[4];

const uint16_t LedPin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 |
                    GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

const uint8_t SegmentData[12] =
{
    0B11000000,                  // 0
    0B11111001,                  // 1
    0B10100100,                  // 2
    0B10110000,                  // 3
    0B10011001,                  // 4
    0B10010010,                  // 5
    0B10000010,                  // 6
    0B11111000,                  // 7
    0B10000000,                  // 8
    0B10010000,                  // 9
    0B10111111,                  // -
    0B11111111                   // "Пусто"
};

static void ConfigLed(void)
{
    // Вкл. порт GPIOB и модуль альтернативных функций.
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE );
    GPIO_PinRemapConfig( GPIO_Remap_SWJ_JTAGDisable, ENABLE );

    // Настройка GPIO.
    GPIO_InitTypeDef GPIO;

    GPIO.GPIO_Pin = LedPin;
    GPIO.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO); // Настройка порта.

    // Настройка DMA. Вкл. модуль DMA1.
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef Dma;

    Dma.DMA_MemoryBaseAddr = (uint32_t) DmaBuff;           // Источник данных массив DmaBuff.
    Dma.DMA_PeripheralBaseAddr = (uint32_t) &(GPIOB->BSRR); // Приемник - регистр BSRR порта GPIOB.
    Dma.DMA_DIR = DMA_DIR_PeripheralDST;                   // Периферия является приемником данных.
    Dma.DMA_BufferSize = 4;                                // Размер источника данных в "словах".
    Dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;     // Не изменять адрес приемника.
    Dma.DMA_MemoryInc = DMA_MemoryInc_Enable;              // Увеличивать адрес памяти.
    Dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // Размер "слова" - 4 байта.
    Dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;         // Размер "слова" - 4 байта.
    Dma.DMA_Mode = DMA_Mode_Circular;                      // Циклическая работа.
    Dma.DMA_Priority = DMA_Priority_Low;                   // Приоритет канала DMA.
    Dma.DMA_M2M = DMA_M2M_Disable;                         // Не копирование из памяти в память.
    DMA_Init(DMA1_Channel5, &Dma);

    // Вкл. таймер TIM1.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // Настройка таймера.
    TIM_TimeBaseInitTypeDef Tim;
    TIM_TimeBaseStructInit(&Tim);

    Tim.TIM_Prescaler = SystemCoreClock / 10000 - 1; // Настройка предделителя таймера.
    Tim.TIM_Period = 10000 / 400 - 1;                // Переполнение таймера 400 раз в секунду.

    TIM_TimeBaseInit(TIM1, &Tim);

    TIM_GenerateEvent(TIM1, TIM_EventSource_Update); // Генерация события при переполнении таймера.
    TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);        // Разрешаем запуск DMA по событию переполнения.
    DMA_Cmd(DMA1_Channel5, ENABLE);                  // Разрешаем работу 2 канала модуля DMA1.
    TIM_Cmd(TIM1, ENABLE);                           // Разрешаем работу таймера TIM2.
}


static void SetDmaArr(const uint8_t *LedData)
{
   for ( uint8_t i=0; i<4; i++ )
   {
    // Вычисление комбинации сегментов и сдвиг на 3, т. к. сегменты подключены начиная с PB3.
    uint32_t x = ((SegmentData[LedData[i]] & ((i==1) ? 0B01111111 : 255)) << 3) & (uint32_t)LedPin;

    // Сдвиг на 12 т. к. разряды подключены начиная с PB12.
    x = x | (((~x)&(uint32_t)LedPin)<<16) | (1<<i<<12) ;

    DmaBuff[i] = x; // Запись данных в DMA буфер.
   }
}


int main(void)
{
    SystemCoreClockUpdate();
    SysTim_Init(100);

    ConfigLed();
    OW_Init();

    uint8_t led[4];

    OW_Reset();
    // Измерение температуры.
    OW_WriteArr((uint8_t*) "\xCC\x44", 2);

    uint8_t ow_buff[9];

    while(1)
    {
        SysTim_DelayMS(800);

        if (OW_Reset() == OW_OK)
        {
            // Считывание температуры.
            OW_WriteArr((uint8_t*) "\xCC\xBE", 2);
            OW_ReadArr(ow_buff, 9);

            if (OW_CalcCrc8(ow_buff, 8)==ow_buff[8])
            {
               float Termo = ((int16_t)(ow_buff[1]<<8)|ow_buff[0])/16.0f;
               int16_t x = Termo*10;

               if (x>=0) led[3] = 11;
               else
               {
                   led[3] = 10;
                   x=(~x)+1;
               }

               led[0] = (uint8_t) (x % 10); x/=10;
               led[1] = (uint8_t) (x % 10); x/=10;
               led[2] = (uint8_t) (x % 10);

               SetDmaArr(led);

            } else goto err;
        }
        else
        {
            err:;
            uint8_t i;
            for (i=0; i<4; i++) led[i] = 10;
            SetDmaArr(led);

        }
        OW_Reset();
        // Измерение температуры.
        OW_WriteArr((uint8_t*) "\xCC\x44", 2);
    }
}
