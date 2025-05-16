#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_uart.h>
#include <stm32f1xx_hal_usart.h>
#include "xprintf.h"

/// Используемый последовательный порт.
#define UART huart1

void Error_Handler( void );

UART_HandleTypeDef huart1;

void xputc( char c )
{
    HAL_UART_Transmit( &UART, ( const uint8_t* ) &c, 1, 1 );
}


/* USART1 init function */
void MX_USART1_UART_Init( void )
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 57600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    if ( HAL_UART_Init( &huart1 ) != HAL_OK )
    {
        Error_Handler();
    }
}


void HAL_UART_MspInit( UART_HandleTypeDef* uartHandle )
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    if ( uartHandle->Instance == USART1 )
    {
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

        HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );

        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;

        HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );

        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority( USART1_IRQn, 0, 0 );
        HAL_NVIC_EnableIRQ( USART1_IRQn );
    }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler( void )
{
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();

    while ( 1 )
    {
    }
}
