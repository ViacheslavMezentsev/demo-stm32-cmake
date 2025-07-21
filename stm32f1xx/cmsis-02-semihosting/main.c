// Минимальный код Си, использующий только заголовочный файл stm32f1xx.h.
// Без стандартной библиотеки Си и startup кода.
// Таблица векторов оформлена кодом на Си.
#include <stm32f1xx.h>
#include <stdio.h>

/// \brief  Цикл, который выполняется до тех пор, пока указанный бит не станет установленным.
/// \param  sfr  Регистровое поле, в котором проверяется бит.
/// \param  bit  Бит, который проверяется.
#define loop_until_bit_is_set( sfr, bit ) do { } while ( !READ_BIT( sfr, bit ) )

/// \brief  Цикл, который выполняется до тех пор, пока указанный бит не станет сброшенным.
/// \param  sfr  Регистровое поле, в котором проверяется бит.
/// \param  bit  Бит, который проверяется.
#define loop_until_bit_is_clear( sfr, bit ) do { } while ( READ_BIT( sfr, bit ) )

/// Semihosting Initializing.
extern void initialise_monitor_handles( void );

/// Указатель стека, определённый в .ld файле.
extern unsigned _estack;
extern unsigned _sidata;    // начало секции .data в flash
extern unsigned _sdata;     // начало секции .data в RAM
extern unsigned _edata;     // окончание секции .data в RAM
extern unsigned _sbss;      // начало секции .bss в RAM
extern unsigned _ebss;      // окончание секции .bss в RAM

__attribute__( ( used, naked ) ) void Reset_Handler( void );
void delay( unsigned udelay );
void SysTick_Handler( void );

/// Тип элемента таблицы векторов прерываний.
typedef void ( *const vector )( void );

/// Таблица векторов прерываний.
__attribute( ( used, section( ".isr_vector" ) ) ) const vector isr_handlers[2 - ( int ) NonMaskableInt_IRQn] =
{
    ( void* ) &_estack,    // начальный указатель стека
    [1] = Reset_Handler,
    [15] = SysTick_Handler
};

volatile unsigned sys_tick_counter = 0;

/***
 *  \brief  Обработчик прерывания SysTick.
 */
void SysTick_Handler()
{
    sys_tick_counter++;
}


/*** 
 *  \brief  Инициализация SysTick для генерации прерывания каждую 1 мс.
 */
void init( void )
{
    // Инициализация библиотеки semihosting.
    // Это необходимо для работы printf и других функций semihosting.
    initialise_monitor_handles();

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

    // Включить тактирование порта C.
    SET_BIT( RCC->APB2ENR, RCC_APB2ENR_IOPCEN );

    // Настроить PC13 как выход push-pull, 2 МГц
    // Очистить MODE13 и CNF13, MODE13 = 0b10 (2 МГц), CNF13 = 0b00 (push-pull).
    MODIFY_REG( GPIOC->CRH, GPIO_CRH_MODE13_Msk | GPIO_CRH_CNF13_Msk,
        ( 0x2U << GPIO_CRH_MODE13_Pos ) | ( 0x0U << GPIO_CRH_CNF13_Pos ) );

    // Отключить SysTick перед настройкой.
    CLEAR_BIT( SysTick->CTRL, SysTick_CTRL_ENABLE_Msk );

    // Загрузить значение для 1 мс.
    WRITE_REG( SysTick->LOAD, ( SystemCoreClock / 1000U ) - 1U );

    // Сбросить текущее значение счетчика.
    WRITE_REG( SysTick->VAL, RESET );

    // Включить SysTick с прерыванием
    WRITE_REG( SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk );
}


/***
 *  \brief  Точка входа в пользовательский код.
 */
int main()
{
    // Инициализация SysTick.
    init();

    while ( 1 )
    {
        // Задержка 500 мс
        delay( 500 );

        // Переключить состояние PC13.
        GPIOC->ODR ^= GPIO_ODR_ODR13;

        // Считываем текущее состояние PC13 и выводим его в консоль.
        // Если светодиод включен, то выводим "On", иначе "Off".
        if ( READ_BIT( GPIOC->ODR, GPIO_ODR_ODR13 ) )
        {            
            printf( "Led On\n" );
        }
        else
        {
            printf( "Led Off\n" );
        }
    }
}


/***
 *  \brief  Выполняет синхронную задержку.
 *  \param  udelay  Количество миллисекунд задержки.
 */
void delay( unsigned udelay )
{
    unsigned start = sys_tick_counter;

    // Ждать, пока не пройдет указанное количество миллисекунд
    // sys_tick_counter увеличивается в SysTick_Handler каждую 1 мс    
    while ( ( sys_tick_counter - start ) < udelay )
    {
        // Ждать
    }
}


/***
 * \brief   Описывает обработчик прерывания вектора сброса мк.
 */
void Reset_Handler()
{
    /*
     Далее необходимо:
     - обнулить секцию bss,
     - скопировать данные глобальных переменных из flash в SRAM,
     - затем вызвать main().
    */

    // Копирование .data из flash в RAM
    unsigned* src = &_sidata;
    unsigned* dst = &_sdata;

    while ( dst < &_edata )
    {
        *dst++ = *src++;
    }

    // Обнуление .bss
    dst = &_sbss;

    while ( dst < &_ebss )
    {
        *dst++ = 0;
    }

    SystemInit();

    main();

    while ( 1 )
    {
    }
}
