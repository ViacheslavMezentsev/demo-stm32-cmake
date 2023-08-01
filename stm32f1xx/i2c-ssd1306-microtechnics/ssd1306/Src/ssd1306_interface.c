/**
  ******************************************************************************
  * @file           : ssd1306_interface.c
  * @brief          : SSD1306 driver interface part
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/

#include "ssd1306_interface.h"


/* Declarations and definitions ----------------------------------------------*/

extern I2C_HandleTypeDef hi2c1;
extern SSD1306_State     SSD1306_state;


/* Functions -----------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void SendCommand( uint8_t* data, uint8_t size )
{
    HAL_I2C_Mem_Write( &hi2c1,
                       SSD1306_I2C_ADDRESS,
                       SSD1306_I2C_CONTROL_BYTE_COMMAND,
                       1,
                       data,
                       size,
                       SSD1306_I2C_TIMEOUT );
}


/*----------------------------------------------------------------------------*/
void SendData( uint8_t* data, uint16_t size )
{
    HAL_I2C_Mem_Write_IT( &hi2c1,
                          SSD1306_I2C_ADDRESS,
                          SSD1306_I2C_CONTROL_BYTE_DATA,
                          1,
                          data,
                          size );
    SSD1306_state = SSD1306_BUSY;
}


/*----------------------------------------------------------------------------*/
void HAL_I2C_MemTxCpltCallback( I2C_HandleTypeDef* hi2c )
{
    SSD1306_state = SSD1306_READY;
}


/*----------------------------------------------------------------------------*/
