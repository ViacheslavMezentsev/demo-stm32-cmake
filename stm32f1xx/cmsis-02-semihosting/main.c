// Минимальный код Си, использующий только заголовочный файл stm32f1xx.h.
// Без стандартной библиотеки Си и startup кода.
// Таблица векторов оформлена кодом на Си.
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <stm32f1xx.h>
#include "xprintf.h"
#include "main.h"

/// Определяем макросы для использования xprintf и xputs.
/// Эти макросы позволяют использовать printf и print как синонимы для xprintf и xputs соответственно.
#ifdef XF_USE_OUTPUT
#define printf xprintf
#define print  xputs
#endif

/// Semihosting Initializing.
extern void initialise_monitor_handles( void );

/// Таблица векторов прерываний.
__attribute( ( used, section( ".isr_vector" ) ) )
const vector isr_handlers[2 - ( int ) NonMaskableInt_IRQn] = { ( void* ) &_estack,    // начальный указатель стека
    [1] = Reset_Handler, [15] = SysTick_Handler };

volatile unsigned sys_tick_counter = 0;

/**
  * @brief  Returns first word of the unique device identifier (UID based on 96 bits)
  * @retval Device identifier
  */
uint32_t HAL_GetUID( uint8_t w )
{
    return ( READ_REG( *( ( uint32_t* ) ( UID_BASE + ( 1 << w ) ) ) ) );
}

/**
 * @brief Проверяет, исполняется ли код в эмуляторе QEMU.
 * @return true, если это QEMU (UID равен нулю), иначе false.
 */
bool is_running_in_qemu( void )
{
    // HAL_GetUIDwX() читает 3 слова (96 бит) уникального ID из специальной
    // области памяти. На реальном железе эти значения уникальны и не равны нулю.
    // QEMU, как правило, возвращает для этой области нули.
    if ( HAL_GetUID(0) == 0x00 && HAL_GetUID(1) == 0x00 && HAL_GetUID(2) == 0x00 )
    {
        return true;
    }

    return false;
}

/**
 * @brief Функция-адаптер для вывода одного символа через Semihosting.
 *        Использует стандартную функцию putchar(), которая при линковке
 *        с rdimon автоматически отправляет данные отладчику.
 * @param c Символ для вывода.
 */
extern void semihosting_putchar( int c )
{
    putchar( c );
}

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
   
    if ( !is_running_in_qemu() )
    {
        // Включить внешний кварцевый генератор HSE.
        SET_BIT( RCC->CR, RCC_CR_HSEON );

        // Ждать, пока HSE не готов.
        loop_until_bit_is_set( RCC->CR, RCC_CR_HSERDY );

        // Выбрать HSE как источник системного тактирования.
        MODIFY_REG( RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSE );

        // Ждать, пока HSE не станет системным тактовым сигналом.
        loop_until_bit_is_set( RCC->CFGR, RCC_CFGR_SWS_HSE );
    }

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

void print_host_time()
{
    time_t raw_time = time( NULL );

    if ( raw_time != -1 )
    {
        struct tm* time_info = localtime( &raw_time );

        // asctime добавляет \n в конце.
        printf( "Host time is: %s", asctime( time_info ) );
    }
}

/***
 *  \brief  Точка входа в пользовательский код.
 */
int main()
{
    // Инициализация SysTick.
    init();

    // Говорим библиотеке xprintf, что для вывода каждого символа
    // нужно вызывать нашу функцию semihosting_putchar.
    xdev_out( semihosting_putchar );

    print_host_time();

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
