#include <stdio.h>
#include <stm32h7xx_hal.h>

/// Semihosting Initializing.
extern "C" void initialise_monitor_handles( void );

/// Обработчик прерывания SysTick.
extern "C" void SysTick_Handler( void )
{
    HAL_IncTick();
}


/**
 * \brief   Точка входа в программу.
 *
 */
int main()
{
    // Инициализация библиотеки Semihosting.
    initialise_monitor_handles();

    // Инициализация библиотеки HAL.
    HAL_Init();

    while ( 1 )
    {
        HAL_Delay( 500 );

        printf( "Hello from STM32!\n" );
    }

    return 0;
}
