#include "stm32f10x.h"
#include "SysTimer.h"

extern uint32_t SystemCoreClock;
volatile uint32_t SysTimer_Counter;
volatile uint16_t SysTimer_Step;

uint32_t SysTim_GetTick(void) // Количество миллисекунд прошедших с момента запуска МК.
{
  return SysTimer_Counter;
}

void SysTim_DelayMS(uint32_t Delay) // Задержка на указанное число миллисекунд.
{
  uint32_t start = 0;
  start = SysTimer_Counter;
  while((SysTimer_Counter - start) < Delay)
  {
      //__WFI();
  }
}

uint8_t SysTim_Init(uint16_t Tick) // Настройка системного таймера.
{                                  // Tick - число тиков (прерываний) в секунду.
    uint8_t Res = 0;
    if ((Tick>4) && (Tick<=1000))
    {
        SysTimer_Counter=0;
        SysTimer_Step = 1000 / Tick;
        if (SysTimer_Step !=0)
        {
            if (SysTick_Config(SystemCoreClock / Tick) == 0)
            {
                Res = 1;
            }
        }
    }
    return Res;
}

void SysTick_Handler(void) // Обработчик прерываний от системного таймера.
{
  SysTimer_Counter+=SysTimer_Step;
}

