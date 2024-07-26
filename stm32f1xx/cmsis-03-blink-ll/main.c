// Минимальный код Си, использующий только заголовочный файл stm32f103x6.h.
// Без стандартной библиотеки Си и startup кода.
// Таблица векторов оформлена кодом на Си.

#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"

#define LED_USER_Pin       LL_GPIO_PIN_13
#define LED_USER_GPIO_Port GPIOC

void delay( unsigned udelay );
__attribute__( ( used, naked ) ) void Reset_Handler( void );
void SysTick_Handler( void );

/// Semihosting Initializing.
extern void initialise_monitor_handles( void );

/// Указатель стека, определённый в .ld файле.
extern unsigned _estack;

/// Тип элемента таблицы векторов прерываний.
typedef void ( *const vector )( void );

/// Таблица векторов прерываний.
__attribute( ( used, section( ".isr_vector" ) ) ) const vector isr_handlers[1 + 15] =
{
    (void*) &_estack,
    [1] = Reset_Handler,
    [15] = SysTick_Handler
};

volatile unsigned uTick;

void setup( void )
{
#ifdef USE_FULL_LL_DRIVER

    // Включение тактирования порта GPIOC.
    LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_GPIOC );

    // Сброс состояния вывода порта.
    LL_GPIO_ResetOutputPin( LED_USER_GPIO_Port, LED_USER_Pin );

    // Установка режима порта.
    LL_GPIO_SetPinMode( LED_USER_GPIO_Port, LED_USER_Pin, LL_GPIO_MODE_OUTPUT );

    // Настройка быстродействия порта.
    LL_GPIO_SetPinSpeed( LED_USER_GPIO_Port, LED_USER_Pin, LL_GPIO_SPEED_FREQ_LOW );

    // Настройка типа выхода порта.
    LL_GPIO_SetPinOutputType( LED_USER_GPIO_Port, LED_USER_Pin, LL_GPIO_OUTPUT_PUSHPULL );

#else

    // Включение тактирования порта GPIOC.
    SET_BIT( RCC->APB2ENR, LL_APB2_GRP1_PERIPH_GPIOC );

    // Сброс состояния вывода порта.
    WRITE_REG( GPIOx->BRR, ( LED_USER_Pin >> GPIO_PIN_MASK_POS ) & 0x0000FFFFU );

    // Установка режима порта.
    // ...

    // Настройка быстродействия порта.
    // ...

    // Настройка типа выхода порта.
    // ...

#endif


    uTick = 0;

    SysTick_Config( 8000000UL / 1000U );
}


/***
 *  \brief  Точка входа в пользовательский код.
 */
int main()
{
    setup();

    while ( 1 )
    {
        if ( uTick > 500 )
        {
            uTick = 0;

            // Переключаем выход порта (мигаем светодиодом).
            LL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );
        }
    }
}


/***
 *  \brief  Выполняет синхронную задержку.
 */
void delay( unsigned udelay )
{
    volatile unsigned count = 0;

    while ( count++ < udelay )
    {
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
     - вызвать глобальные конструкторы,
     - затем вызвать main().
    */

    main();

    while ( 1 )
    {
    }
}


/**
 *  \brief This function handles System tick timer.
 */
void SysTick_Handler( void )
{
    uTick++;
}
