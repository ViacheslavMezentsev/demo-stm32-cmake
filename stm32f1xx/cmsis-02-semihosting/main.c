// Минимальный код Си, использующий только заголовочный файл stm32f103x6.h.
// Без стандартной библиотеки Си и startup кода.
// Таблица векторов оформлена кодом на Си.

#include <stm32f103x6.h>

void delay( unsigned udelay );
__attribute__( ( used, naked ) ) void Reset_Handler( void );

/// Semihosting Initializing.
extern void initialise_monitor_handles( void );

/// Указатель стека, определённый в .ld файле.
extern unsigned _estack;

/// Тип элемента таблицы векторов прерываний.
typedef void ( *const vector )( void );

/// Таблица векторов прерываний.
__attribute( ( used, section( ".isr_vector" ) ) )
const vector isr_handlers[1 + 14] = { (void*) &_estack, Reset_Handler };

/***
 *  \brief  Точка входа в пользовательский код.
 */
int main()
{
    // Инициализация библиотеки Semihosting.
    initialise_monitor_handles();

    while ( 1 )
    {
        delay( 100000 );
    }
}


/***
 *  \brief  Выполняет синхронную задержку.
 */
void delay( unsigned udelay )
{
    volatile unsigned count = 0;

    while ( count++ < udelay ) {}
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

    while ( 1 ) {}
}
