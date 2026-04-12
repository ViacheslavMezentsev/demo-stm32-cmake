#ifndef QEMU_H
#define QEMU_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief  Адрес регистра CPUID в блоке System Control Block (SCB).
 * @note   Этот адрес является стандартным для всех ядер Cortex-M.
 */
#define SCB_CPUID_ADDR ( 0xE000ED00UL )
#define SCB_CPUID      ( *( ( volatile const uint32_t* ) SCB_CPUID_ADDR ) )

#define DBGMCU_IDCODE       (*(volatile uint32_t*)0xE0042000)
#define DEV_ID_MASK         0x00000FFF
#define DEV_ID_STM32F411    0x431

// Макрос для определения, запущено ли приложение в QEMU или на реальном железе.
#define IS_RUNNING_IN_QEMU()  ((DBGMCU_IDCODE & DEV_ID_MASK) != DEV_ID_STM32F411)

void qemu_exit( int code );
void qemu_print( char *ptr, int len );

#ifdef __cplusplus
}
#endif

#endif // QEMU_H