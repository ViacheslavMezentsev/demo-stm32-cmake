#include <stdio.h>
#include <stm32h7xx.h>
#include "etl/string.h"

/// Semihosting Initializing.
extern "C" void initialise_monitor_handles( void );

etl::string<50> hello( "Hello from STM32!" );

int main( void )
{
    // Инициализация библиотеки Semihosting.
    initialise_monitor_handles();

    printf( "%s\n", hello.c_str() );

    while (1) {}

    return 0;
}
