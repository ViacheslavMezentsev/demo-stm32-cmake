#include "interpreter.h"
#include "picoc.h"
#include "hardware.h"
#include "usart.h"
#include "gpio.h"

/*
#ifdef __DBG_ITM
volatile int ITM_RxBuffer = ITM_RXBUFFER_EMPTY;
#endif

int fputc(int ch, FILE *f)
{
#ifdef __DBG_ITM
    ITM_SendChar(ch);
#endif
	return (ch);
}

char getch(void)
{
#ifdef __DBG_ITM
	while (ITM_CheckChar() != 1)
		__NOP();
	return (char)(ITM_ReceiveChar());
#else
	while(1);
#endif
}
*/

void fputcEx( int ch )
{
#ifdef CONSOLE_PORT
    // Включаем прерывания передачи данных.
    __HAL_UART_ENABLE_IT( &huart1, UART_IT_TXE );
#endif
}

#ifdef CONSOLE_PORT

/**
 * @brief   Обработчик прерывания USART1.
 * 
 * @note    Обрабатывает прерывания передачи и приема данных по USART1.
 */
void Platform_USART1_IRQHandler( void )
{
    uint8_t ch;

    /* Transmit data register empty interrupt */
    if ( __HAL_UART_GET_IT_SOURCE( &huart1, UART_IT_TXE ) != RESET )
    {
        if ( ConsoleTxBuffer.idx_in != ConsoleTxBuffer.idx_out )
        {
            HAL_UART_Transmit_IT( &huart1, &ConsoleTxBuffer.data[ConsoleTxBuffer.idx_out++], 1 );
            ConsoleTxBuffer.idx_out &= ( CONSOLE_BUFFER_SIZE - 1 );
        } else
            __HAL_UART_DISABLE_IT( &huart1, UART_IT_TXE );
    }

    /* Read data register not empty interrupt */
    if ( __HAL_UART_GET_IT_SOURCE( &huart1, UART_IT_RXNE ) != RESET )
    {
        uint16_t next = ( ConsoleRxBuffer.idx_in + 1 ) & ( CONSOLE_BUFFER_SIZE - 1 );

        // Если буфер не переполнен, читаем данные из регистра данных USART.
        if ( next != ConsoleRxBuffer.idx_out )
        {
            HAL_UART_Receive_IT( &huart1, &ch, 1 );

            if ( ch != 0 )
            {
                ConsoleRxBuffer.data[ConsoleRxBuffer.idx_in] = ch;
                ConsoleRxBuffer.idx_in = next;
            }
        }
    }
}
#endif

/**
 * @brief   Инициализация платформы.
 * 
 * @param pc    Указатель на структуру Picoc.
 */
void PlatformInitEx( Picoc* pc )
{
    // Настройка тактирования системы.
    SystemClock_Config();

    // Инициализация GPIO.
    MX_GPIO_Init();

    // Инициализация USART1.
    MX_USART1_UART_Init();
}


/**
 * @brief   Очищает ресурсы платформы.
 * 
 * @param pc 
 */
void PlatformCleanupEx( Picoc* pc )
{
    HAL_UART_MspDeInit( &huart1 );
}

/**************************************************************************/ /**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter.
 *****************************************************************************/
volatile unsigned long PlatformDelayMs = 0;
volatile unsigned long PlatformMsTicks = 0;

void PlatformTick_Handler( void )
{
    /* Increment counter necessary in Delay()*/
    PlatformMsTicks++;

    if ( PlatformDelayMs > 0 ) PlatformDelayMs--;
}
