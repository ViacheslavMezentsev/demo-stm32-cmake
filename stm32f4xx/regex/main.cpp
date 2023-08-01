#include <stdio.h>
#include <stm32f4xx_hal.h>

/// Semihosting Initializing.
extern "C" void initialise_monitor_handles( void );

extern "C" void test_search( void );

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

    // Пример использования библиотеки tiny-regex-с.
    test_search();

    while ( 1 ) { }

    return 0;
}
