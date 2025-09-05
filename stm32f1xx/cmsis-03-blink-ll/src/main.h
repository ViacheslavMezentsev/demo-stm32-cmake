#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <time.h>
#include <stm32f1xx.h>
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* the Field MaKe (Raw) macro */
// Этот макрос используется для создания битового поля в регистре.
// Пример: CSL_FMKR(23, 22, 0x2) создаст поле из 2 битов, начиная с бита 22 до бита 23, со значением 0x2.
#define CSL_FMKR(msb, lsb, val)                                             \
    (((val) & ((1 << ((msb) - (lsb) + 1)) - 1)) << (lsb))

/* the Field EXTract (Raw) macro */
// Этот макрос используется для извлечения битового поля из регистра.
// Пример: CSL_FEXTR(reg, 23, 22) извлечет поле из 2 битов, начиная с бита 22 до бита 23 из регистра reg.
#define CSL_FEXTR(reg, msb, lsb)                                            \
    (((reg) >> (lsb)) & ((1 << ((msb) - (lsb) + 1)) - 1))

/* the Field INSert (Raw) macro */
// Этот макрос используется для вставки значения в битовое поле регистра.
// Пример: CSL_FINSR(reg, 23, 22, 0x2) вставит значение 0x2 в поле из 2 битов, начиная с бита 22 до бита 23 в регистр reg.
#define CSL_FINSR(reg, msb, lsb, val)                                       \
    ((reg) = ((reg) &~ (((1 << ((msb) - (lsb) + 1)) - 1) << (lsb)))         \
    | CSL_FMKR(msb, lsb, val))


/// \brief  Цикл, который выполняется до тех пор, пока указанный бит не станет установленным.
/// \param  sfr  Регистровое поле, в котором проверяется бит.
/// \param  bit  Бит, который проверяется.
#define loop_until_bit_is_set( sfr, bit ) do { } while ( !READ_BIT( sfr, bit ) )

/// \brief  Цикл, который выполняется до тех пор, пока указанный бит не станет сброшенным.
/// \param  sfr  Регистровое поле, в котором проверяется бит.
/// \param  bit  Бит, который проверяется.
#define loop_until_bit_is_clear( sfr, bit ) do { } while ( READ_BIT( sfr, bit ) )

__attribute__( ( used, naked ) ) void Reset_Handler( void );
void delay( unsigned udelay );
void SysTick_Handler( void );

#if !defined  (HSI_VALUE)
  #define HSI_VALUE               8000000U /*!< Default value of the Internal oscillator in Hz.
                                                This value can be provided and adapted by the user application. */
#endif /* HSI_VALUE */

/// Указатель стека, определённый в .ld файле.
extern unsigned _estack;
extern unsigned _sidata;    // начало секции .data в flash
extern unsigned _sdata;     // начало секции .data в RAM
extern unsigned _edata;     // окончание секции .data в RAM
extern unsigned _sbss;      // начало секции .bss в RAM
extern unsigned _ebss;      // окончание секции .bss в RAM

/// Тип элемента таблицы векторов прерываний.
typedef void ( *const vector )( void );

/// Определяем макросы для использования xprintf и xputs.
/// Эти макросы позволяют использовать printf и print как синонимы для xprintf и xputs соответственно.
#ifdef XF_USE_OUTPUT
#define printf xprintf
#define print  xputs
#endif

/**
  * @brief  Макросы для прямого чтения 96-битного Unique Device ID.
  */
#define STM32_UID_WORD0     ( *( (volatile uint32_t *) (UID_BASE + 0x00) ) )
#define STM32_UID_WORD1     ( *( (volatile uint32_t *) (UID_BASE + 0x04) ) )
#define STM32_UID_WORD2     ( *( (volatile uint32_t *) (UID_BASE + 0x0C) ) )

/**
  * @brief  (ТЕОРЕТИЧЕСКИЙ) Макрос для проверки, исполняется ли код в эмуляторе QEMU.
  * @retval 1 если UID равен нулю, 0 в противном случае.
  */
#define IS_RUNNING_IN_QEMU() ( (STM32_UID_WORD0 == 0x00U) && \
    (STM32_UID_WORD1 == 0x00U) && (STM32_UID_WORD2 == 0x00U) )

/// Semihosting Initializing.
extern void initialise_monitor_handles( void );

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
