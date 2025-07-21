// Минимальный код Си, использующий только CMSIS.
// Без стандартной библиотеки Си и startup кода.
// Таблица векторов оформлена кодом на Си.
#include <stm32f1xx.h>
#include <cmsis_version.h>
#include "SEGGER_RTT.h"
#include "main.h"

// SEGGER RTT: IP: localhost, PORT: 19021.
#define print(s)                        SEGGER_RTT_WriteString( 0, s ); delay(1)
#define println(s)                      print( s "\n" )
#define printf( format, ... )           SEGGER_RTT_printf( 0, ( const char * ) ( format ), ##__VA_ARGS__ ); delay(1)

/// Таблица векторов прерываний.
__attribute( ( used, section( ".isr_vector" ) ) ) const vector isr_handlers[2 - ( int ) NonMaskableInt_IRQn] =
{
    ( void* ) &_estack,    // начальный указатель стека
    [1] = Reset_Handler,
    [15] = SysTick_Handler
};

volatile unsigned sys_tick_counter = 0;

// Переменные для проверки инициализации секций .data и .bss.
// Инициализированная переменная до инициализации будет иметь произвольное значение,
// после инициализации будет иметь значение 0x12345678.
// Неинициализированная переменная до инициализации может иметь значение 0xFFFFFFFF,
// после инициализации будет иметь значение 0 (нулевое).
int init_data_check = 0x12345678;   // Инициализированная переменная в .data
int uninit_data_check;              // Неинициализированная переменная в .bss

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

#if defined (__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U)
    SysTick_Config( SystemCoreClock / 1000U );
#else
    // Отключить SysTick перед настройкой.
    CLEAR_BIT( SysTick->CTRL, SysTick_CTRL_ENABLE_Msk );

    // Загрузить значение для 1 мс.
    WRITE_REG( SysTick->LOAD, ( SystemCoreClock / 1000U ) - 1U );

    // Сбросить текущее значение счетчика.
    WRITE_REG( SysTick->VAL, RESET );

    // Включить SysTick с прерыванием.
    WRITE_REG( SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk );
#endif

    init_data_check = -1;
    uninit_data_check = -1;

    // Настройка терминала 0 для работы в неблокирующем режиме.
    SEGGER_RTT_ConfigUpBuffer( 0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM );

    println( "SEGGER Real-Time-Terminal Sample" );

    // Вывод версии CMSIS.
    printf( "CMSIS Version: %u.%u\n", __CM_CMSIS_VERSION_MAIN, __CM_CMSIS_VERSION_SUB );

    // Вывод версии компилятора.
    printf( "Compiler Version: %s\n", __VERSION__ );

    // Вывод информации о системе.
    printf( "SystemCoreClock: %lu Hz\n", SystemCoreClock );

    // Вывод UID.
    volatile uint32_t* uid = ( uint32_t* ) UID_BASE;
    printf("UID: %08lX-%08lX-%08lX\n", uid[2], uid[1], uid[0]);
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
            println( "Led On" );
        }
        else
        {
            println( "Led Off" );
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
    }
}


/***
 * \brief   Описывает обработчик прерывания вектора сброса мк.
 */
void Reset_Handler()
{
    // Копирование .data из flash в RAM (секция для инициализированных глобальных переменных).
    // _sidata - начало секции .data в flash,
    // _sdata - начало секции .data в RAM,
    // _edata - окончание секции .data в RAM.
    unsigned* src = &_sidata;
    unsigned* dst = &_sdata;

    while ( dst < &_edata )
    {
        *dst++ = *src++;
    }

    // Обнуление .bss (секция для неинициализированных глобальных переменных).
    dst = &_sbss;

    while ( dst < &_ebss )
    {
        *dst++ = 0;
    }

    // Вызов функции инициализации системы.
    SystemInit();

    // Вызов основной функции приложения.
    main();
}
