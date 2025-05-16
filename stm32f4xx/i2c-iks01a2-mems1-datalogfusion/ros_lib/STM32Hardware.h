/*
 * STM32Hardware.h
 *
 *  Created on: Nov 2, 2019
 *      Author: xav-jann1
 */

#ifndef ROS_STM32_HARDWARE_H_
#define ROS_STM32_HARDWARE_H_

#include "BufferedSerial.hpp"

/// Create Serial Buffer.
extern BufferedSerial buff_serial;

class STM32Hardware
{
public:
    STM32Hardware() : serial( &buff_serial )
    {
    }

    /// Any initialization code necessary to use the serial port.
    void init()
    {
        serial->init();
    }

    /// Read a byte from the serial port. -1 = failure.
    int read()
    {
        return serial->read();
    }

    /// Write data to the connection to ROS.
    void write( uint8_t* data, int length )
    {
        serial->write( data, length );
    }

    /// Returns milliseconds since start of program.
    unsigned long time()
    {
        return HAL_GetTick();
    };

protected:
    BufferedSerial* serial;
};

#endif
