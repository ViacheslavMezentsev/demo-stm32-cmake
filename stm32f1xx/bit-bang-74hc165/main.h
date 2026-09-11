#ifndef __MAIN_H
#define __MAIN_H

#include <stdbool.h>
#include <stdio.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_uart.h>
#include <stm32f1xx_hal_usart.h>
#include "xprintf.h"

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

void SysTick_Handler( void );

/**
  * @brief  Макросы для прямого чтения 96-битного Unique Device ID.
  */
#define STM32_UID_WORD0 ( *( ( volatile uint32_t* ) ( UID_BASE + 0x00 ) ) )
#define STM32_UID_WORD1 ( *( ( volatile uint32_t* ) ( UID_BASE + 0x04 ) ) )
#define STM32_UID_WORD2 ( *( ( volatile uint32_t* ) ( UID_BASE + 0x08 ) ) )

/**
  * @brief  Проверяет, исполняется ли код в эмуляторе QEMU, сравнивая UID с нулем.
  * @note   Этот макрос не зависит от HAL и может быть вызван до HAL_Init().
  * @retval 1 если UID равен нулю (вероятно, QEMU), 0 в противном случае.
  */
#define IS_RUNNING_IN_QEMU() \
    ( ( STM32_UID_WORD0 == 0x00U ) && ( STM32_UID_WORD1 == 0x00U ) && ( STM32_UID_WORD2 == 0x00U ) )

/// Определяем макросы для использования xprintf и xputs.
/// Эти макросы позволяют использовать printf и print как синонимы для xprintf и xputs соответственно.
#ifdef XF_USE_OUTPUT
#define printf        xprintf
#define print(x)      xputs(x)
#define println(x)    xputs(x "\n")
#endif

extern UART_HandleTypeDef huart1;

/// @brief  Функция для передачи одного символа по последовательному порту.
extern "C" void uart_putc( int c );

/// Пин светодиода (STM32F103C8T6).
#define LED_USER_Pin       GPIO_PIN_13
#define LED_USER_GPIO_Port GPIOC

#ifdef __cplusplus
}
#endif

/// Инициализация USART1.
/// @note    Настройка USART1 производится в файле usart.cpp.
extern void MX_USART1_UART_Init( void );

#endif /* __MAIN_H */
