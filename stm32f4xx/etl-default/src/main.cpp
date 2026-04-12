#include <stdio.h>
#include <stm32f4xx.h>
#include "etl/string.h"
#include "qemu.h"

/// Semihosting Initializing.
extern "C" void initialise_monitor_handles( void );

/** @brief Перенаправление вывода printf в QEMU через Semihosting.
 * @param file Дескриптор файла.
 * @param ptr Указатель на строку для вывода.
 * @param len Длина строки.
 * @return Количество символов, успешно выведенных.
 */
extern "C" int _write( int file, char *ptr, int len )
{
    (void) file;

    if ( IS_RUNNING_IN_QEMU() )
    {
        qemu_print( ptr, len );
        return len;
    }

    return len;
}

// Пример использования ETL string для хранения строки "Hello from STM32!".
etl::string<50> hello( "Hello from STM32!" );

/**
 * @brief Точка входа в программу.
 * 
 */
int main( void )
{   
    if ( !IS_RUNNING_IN_QEMU() )
    {
        SystemCoreClockUpdate();

        // Инициализация библиотеки Semihosting.
        initialise_monitor_handles();
    }

    printf( "%s\n", hello.c_str() );

    if ( IS_RUNNING_IN_QEMU() )
    {
        printf( "Done.\r\n" );

        // QEMU завершится с кодом 0.
        qemu_exit(0);
    }

    while (1) {}
}
