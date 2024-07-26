#include "stm32f10x_it.h"
#include "timer.h"

// Global variable Timer count value.
uint16_t current_clock = 0;

/*
********************************************************************************
* Function name: timer_config
* Function description: Use systick as a clock
* Parameters: none
* Return value: None
* Instructions for use: can be called during power-on initialization
* Calling method: timer_config()
********************************************************************************
*/
void timer_config( void )
{
    /* The Systick clock triggers CLOCK_SECOND times every s */
    if ( SysTick_Config( SystemCoreClock / CLOCK_SECOND ) )
    {
        while ( 1 );
    }
}


/*
********************************************************************************
* Function name: SysTick_Handler
* Function description: Systick interrupt function
* Parameters: none
* Return value: None
* Instructions for use:
* Call method:
********************************************************************************
*/
void SysTick_Handler( void )
{
    /* Time stamp accumulation */
    current_clock++;
}


/*
********************************************************************************
* Function name: clock_time
* Function description: Return the current count value
* Parameters: none
* Return value: uint16_t current_clock current count time
* Instructions for use:
* Calling method: clock_time()
********************************************************************************
*/
uint16_t clock_time( void )
{
    return current_clock;
}


/*
********************************************************************************
* Function name: timer_set
* Function description: Set the timer interval time
* Parameters: timer_typedef* ptimer timer structure
*           uint16_t interval interval time
* Return value: None
* Instructions for use:
* Calling method: timer_set(&arp_timer,10000);
********************************************************************************
*/
void timer_set( timer_typedef * ptimer, uint16_t interval )
{
    /* set time interval */
    ptimer->interval = interval;

    /* Set start time */
    ptimer->start = clock_time();
}


/*
********************************************************************************
* Function name: timer_reset
* Function description: reset the timer
* Parameters: timer_typedef* ptimer timer structure
* Return value: None
* Instructions for use:
* Calling method: timer_reset(&arp_timer);
********************************************************************************
*/
void timer_reset( timer_typedef * ptimer )
{
    ptimer->start = ptimer->start + ptimer->interval;
}


/*
********************************************************************************
* Function name: timer_expired
* Function description: Query whether the timer overflows
* Parameters: timer_typedef* ptimer timer structure
* Return value: int8_t Whether the timer overflows 1 means overflow 0 means not overflow
* Instructions for use:
* Calling method: timer_expired(&arp_timer)
********************************************************************************
*/
int8_t timer_expired( timer_typedef * ptimer )
{
    // Be sure to replace it with a signed number.
    // When performing mathematical comparisons, use more signed numbers.
    return ( ( int16_t )( clock_time() - ptimer->start ) >= ( int16_t ) ptimer->interval ) ? 1 : 0;
}
