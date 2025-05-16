/*
 * BufferedSerial.cpp
 *
 *  Created on: Nov 2, 2019
 *      Author: xav-jann1
 */
#include "BufferedSerial.hpp"

extern UART_HandleTypeDef huart6;

// Create Serial Buffer with UART2:
BufferedSerial buff_serial( huart6 );

/**
 * @brief   Constructor
 * 
 * @param huart_ 
 */
BufferedSerial::BufferedSerial( UART_HandleTypeDef& huart_ )
: huart( huart_ )
{
}


// Init.
void BufferedSerial::init( void )
{
    reset_rx_buffer();
}


// Read new char in RX buffer.
int BufferedSerial::read( void )
{
    // Get DMA head.
    uint16_t dma_head = ( RX_BUF_SIZE - huart.hdmarx->Instance->NDTR ) & rx_buf_mask;

    // Quit if no new character.
    if ( dma_head == rx_tail )
    {
        return -1;
    }

    // Get next char in buffer.
    int c = (int) rx_buf[ rx_tail++ ];

    // Wrap around if rx_tail > RX_BUF_SIZE.
    rx_tail &= rx_buf_mask;

    return c;
}


// Prepare data and send it.
void BufferedSerial::write( const uint8_t* data, const int length )
{
    // If data can fit at the end of the buffer:
    if ( tx_head + length < TX_BUF_SIZE )
    {
        memcpy( &( tx_buf[ tx_head ] ), data, length );

        tx_head += length;    // ) & tx_buf_mask

        // Avoids "wrong checksum for topic id and msg" in rosserial logs.
        if ( tx_head > tx_end )
        {
            tx_end = tx_head;
        }
    }

    // Else data is copied at the beginning of TX buffer.
    else
    {
        memcpy( tx_buf, data, length );

        // Avoids tx_end > tx_tail.
        if ( tx_head > tx_tail )
        {
            tx_end = tx_head;
        }

        tx_head = length;
    }

    // Send data.
    flush_tx_buffer();
}


// Send data.
void BufferedSerial::flush_tx_buffer( void )
{
    static bool mutex = false;

    // Reset indexes if they are at the same position.
    if ( tx_head != 0 && tx_head == tx_tail )
    {    
        // Can be removed (just for better memory management).
        tx_head = 0;
        tx_tail = 0;
    }

    // Quit if UART not ready to transmit data or no data to send.
    if ( huart.gState != HAL_UART_STATE_READY || tx_head == tx_tail || mutex )
    {
        return;
    }

    mutex = true;

    // Reset flush index if already sent complete TX buffer.
    if ( tx_tail == tx_end )
    {
        tx_tail = 0;
    }

    // Send data behind head.
    if ( tx_tail < tx_head )
    {
        uint16_t length = tx_head - tx_tail;

        HAL_UART_Transmit_DMA( &huart, &( tx_buf[ tx_tail ] ), length );

        tx_tail = tx_head;
    }

    // Else end the buffer before resetting tail index.
    else
    {
        uint16_t length = tx_end - tx_tail;
        HAL_UART_Transmit_DMA( &huart, &( tx_buf[ tx_tail ] ), length );

        // Reset indexes.
        tx_end  = TX_BUF_SIZE;
        tx_tail = 0;
    }

    mutex = false;
}


// Reset DMA to the beginning of the RX buffer.
inline void BufferedSerial::reset_rx_buffer( void )
{
    HAL_UART_Receive_DMA( &huart, rx_buf, RX_BUF_SIZE );
}


// Get UART Handle.
UART_HandleTypeDef* const BufferedSerial::get_handle( void )
{
    return &huart;
}


// DMA callbacks.
void HAL_UART_TxCpltCallback( UART_HandleTypeDef* huart )
{
    // Comparing pointers: (remove equality if only one UART is used).
    if ( huart->Instance == buff_serial.get_handle()->Instance )
    {
        buff_serial.flush_tx_buffer();
    }
}


void HAL_UART_RxCpltCallback( UART_HandleTypeDef* huart )
{
    // Can be commented if DMA mode for RX is Circular.
    if ( huart->Instance == buff_serial.get_handle()->Instance )
    {
        buff_serial.reset_rx_buffer();
    }
}
