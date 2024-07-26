#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

#define CLOCK_SECOND  1000

typedef struct
{
    /* 开始时间 */
    uint16_t start;
    /* 时间间隔 */
    uint16_t interval;

} timer_typedef;

/* 外部函数声明 ***************************************************************/
void timer_config( void );
uint16_t clock_time( void );
void timer_set( timer_typedef * ptimer, uint16_t interval );
void timer_reset( timer_typedef * ptimer );
int8_t timer_expired( timer_typedef * ptimer );

#endif
