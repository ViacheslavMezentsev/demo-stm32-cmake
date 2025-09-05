// Пример программы с использованием Low Layer (LL) драйвера.
// Цель: мигание встроенным светодиодом на плате Blue Pill (PC13) с использованием SysTick для задержки.
// Без стандартной библиотеки Си и startup кода.
// Таблица векторов оформлена кодом на Си.
#include "main.h"

#define LED_USER_Pin       LL_GPIO_PIN_13
#define LED_USER_GPIO_Port GPIOC

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
    if ( !IS_RUNNING_IN_QEMU() )
    {
        // Включить внешний кварцевый генератор HSE.
        LL_RCC_HSE_Enable();

        // Ждать, пока HSE не готов.
        while ( !LL_RCC_HSE_IsReady() ) { }

        // Выбрать HSE как источник системного тактирования.
        LL_RCC_SetSysClkSource( LL_RCC_SYS_CLKSOURCE_HSE );

        // Ждать, пока HSE не станет системным тактовым сигналом.
        while ( LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE ) { }
    }

    // Обновление частоты системного тактового генератора.
    SystemCoreClockUpdate();

    // Включение тактирования порта GPIOC.
    LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_GPIOC );

    // Сброс состояния вывода порта.
    LL_GPIO_ResetOutputPin( LED_USER_GPIO_Port, LED_USER_Pin );

    LL_GPIO_InitTypeDef GPIO_InitStruct =
    {
        .Pin = LED_USER_Pin,
        .Mode = LL_GPIO_MODE_OUTPUT,            // Режим порта: выход
        .Pull = LL_GPIO_PULL_DOWN,              // Подтяжка: вниз
        .Speed = LL_GPIO_SPEED_FREQ_LOW,        // Скорость: низкая
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL   // Тип выхода: push-pull
    };

    LL_GPIO_Init( LED_USER_GPIO_Port, &GPIO_InitStruct );

    // Отключить SysTick перед настройкой.
    CLEAR_BIT( SysTick->CTRL, SysTick_CTRL_ENABLE_Msk );

    // Настраиваем SysTick.
    LL_Init1msTick( SystemCoreClock );
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
        LL_mDelay( 500 );

        // Переключаем выход порта (мигаем светодиодом).
        LL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );
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
