#include <stdio.h>
#include <stm32f1xx.h>
#include "etl/string.h"

/// \brief  Цикл, который выполняется до тех пор, пока указанный бит не станет установленным.
/// \param  sfr  Регистровое поле, в котором проверяется бит.
/// \param  bit  Бит, который проверяется.
#define loop_until_bit_is_set( sfr, bit ) do { } while ( !READ_BIT( sfr, bit ) )

/// \brief  Цикл, который выполняется до тех пор, пока указанный бит не станет сброшенным.
/// \param  sfr  Регистровое поле, в котором проверяется бит.
/// \param  bit  Бит, который проверяется.
#define loop_until_bit_is_clear( sfr, bit ) do { } while ( READ_BIT( sfr, bit ) )

/// Инициализация Semihosting для вывода в консоль.
extern "C" void initialise_monitor_handles( void );

/// Строка для вывода в консоль.
/// Используется библиотека ETL для работы со строками.
etl::string<50> hello( "Hello from STM32!" );

/*** 
 *  \brief  Инициализация.
 */
void init( void )
{
    // Включить внешний кварцевый генератор HSE.
    SET_BIT( RCC->CR, RCC_CR_HSEON );

    // Ждать, пока HSE не готов.
    loop_until_bit_is_set( RCC->CR, RCC_CR_HSERDY );

    // Выбрать HSE как источник системного тактирования.
    MODIFY_REG( RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSE );

    // Ждать, пока HSE не станет системным тактовым сигналом.
    loop_until_bit_is_set( RCC->CFGR, RCC_CFGR_SWS_HSE );

    // Обновление частоты системного тактового генератора.
    SystemCoreClockUpdate();

    // Инициализация библиотеки Semihosting.
    initialise_monitor_handles();
}


/**
 * \brief   Точка входа в программу.
 *
 */
int main( void )
{
    // Инициализация.
    init();

    // Вывод строки в консоль.
    // Используется Semihosting для вывода в консоль.
    printf( "%s\n", hello.c_str() );

    while (1) {}
}
