#include <stdio.h>
#include <stm32f4xx.h>
#include "qemu.h"

/// Semihosting Initializing.
extern "C" void initialise_monitor_handles( void );
extern "C" void test_search( void );

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

/**
 * \brief   Точка входа в программу.
 *
 */
int main()
{
    if ( !IS_RUNNING_IN_QEMU() )
    {
        // Инициализация библиотеки Semihosting.
        initialise_monitor_handles();
    }

    // Пример использования библиотеки tiny-regex-с.
    test_search();

    if ( IS_RUNNING_IN_QEMU() )
    {
        printf( "Done.\r\n" );

        // QEMU завершится с кодом 0.
        qemu_exit(0);
    }

    while ( 1 ) {}
}
