/*
 * BufferedSerial.h
 *
 *  Created on: Nov 2, 2019
 *      Author: xav-jann1
 */

#ifndef BUFFEREDSERIAL_H_
#define BUFFEREDSERIAL_H_

#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

#define RX_BUF_SIZE 512
#define TX_BUF_SIZE 4096

class BufferedSerial
{
private:
    UART_HandleTypeDef& huart;

    static constexpr uint16_t rx_buf_mask = RX_BUF_SIZE - 1;
    static constexpr uint16_t tx_buf_mask = TX_BUF_SIZE - 1;
    uint8_t rx_buf[ RX_BUF_SIZE ];
    uint8_t tx_buf[ TX_BUF_SIZE ];

    uint16_t rx_tail = 0;
    uint16_t tx_head = 0;
    uint16_t tx_tail = 0;
    uint16_t tx_end  = TX_BUF_SIZE;

public:
    BufferedSerial( UART_HandleTypeDef& huart );
    BufferedSerial();
    UART_HandleTypeDef* const get_handle( void );

    void init( void );
    int  read( void );
    void write( const uint8_t* const c, const int length );
    void flush_tx_buffer();

    void tx_cplt_callback( void );
    void reset_rx_buffer( void );
};

#endif
