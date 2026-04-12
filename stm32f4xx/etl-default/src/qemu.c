#include <string.h>
#include "qemu.h"

/**
 * @file qemu.c
 * @brief Функции для взаимодействия с QEMU через Semihosting.
 * 
 * Этот файл содержит функции для выхода из QEMU и вывода строк в QEMU через Semihosting.
 */
void qemu_exit( int code )
{
    register int r0 __asm__("r0") = 0x18;  // SYS_EXIT
    register int r1 __asm__("r1") = code;  // 0 = success, иначе код ошибки
    __asm__ volatile (
        "bkpt 0xAB"
        : "+r"(r0) : "r"(r1) : "memory"
    );
}

/**
 * @brief Вывод строки в QEMU через Semihosting.
 * @param ptr Указатель на строку для вывода.
 * @param len Длина строки.
 */
void qemu_print( char *ptr, int len )
{
    // SYS_WRITE0 требует null-terminated строку
    char buf[len + 1];
    memcpy( buf, ptr, len );
    buf[len] = '\0';

    register int    r0 __asm__("r0") = 0x04;  // SYS_WRITE0
    register char * r1 __asm__("r1") = buf;
    __asm__ volatile (
        "bkpt 0xAB"
        : "+r"(r0) : "r"(r1) : "memory"
    );
}