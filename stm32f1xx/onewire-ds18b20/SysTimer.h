#ifndef SYSTIMER_H_INCLUDED
#define SYSTIMER_H_INCLUDED

uint32_t SysTim_GetTick(void);
void SysTim_DelayMS(uint32_t Delay);
uint8_t SysTim_Init(uint16_t Tick);

#endif /* SYSTIMER_H_INCLUDED */
