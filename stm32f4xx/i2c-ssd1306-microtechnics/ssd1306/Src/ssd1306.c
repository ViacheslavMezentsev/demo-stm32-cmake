/**
  ******************************************************************************
  * @file           : ssd1306.c
  * @brief          : SSD1306 driver
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/

#include "ssd1306.h"
#include "ssd1306_interface.h"


/* Declarations and definitions ----------------------------------------------*/

SSD1306_State SSD1306_state = SSD1306_READY;

static uint8_t pixelBuffer[SSD1306_BUFFER_SIZE];


/* Functions -----------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void SSD1306_Init()
{
    uint8_t data[3];

    // Set display off
    data[0] = 0xAE;
    SendCommand( data, 1 );

    // Set oscillator frequency
    data[0] = 0xD5;
    data[1] = 0x80;
    SendCommand( data, 2 );

    // Enable charge pump regulator
    data[0] = 0x8D;
    data[1] = 0x14;
    SendCommand( data, 2 );

    // Set display start line
    data[0] = 0x40;
    SendCommand( data, 1 );

    // Set segment remap
    data[0] = 0xA1;
    SendCommand( data, 1 );

    // Set COM output scan direction
    data[0] = 0xC8;
    SendCommand( data, 1 );

    // Set COM pins hardware configuration
    data[0] = 0xDA;
    data[1] = 0x12;
    SendCommand( data, 2 );

    // Set MUX ratio
    data[0] = 0xA8;
    data[1] = 63;
    SendCommand( data, 2 );

    // Set display offset
    data[0] = 0xD3;
    data[1] = 0;
    SendCommand( data, 2 );

    // Set horizontal addressing mode
    data[0] = 0x20;
    data[1] = 0x00;
    SendCommand( data, 2 );

    // Set column address
    data[0] = 0x21;
    data[1] = 0;
    data[2] = 127;
    SendCommand( data, 3 );

    // Set page address
    data[0] = 0x22;
    data[1] = 0;
    data[2] = 7;
    SendCommand( data, 3 );

    // Set contrast
    data[0] = 0x81;
    data[1] = 0x7F;
    SendCommand( data, 2 );

    // Entire display on
    data[0] = 0xA4;
    SendCommand( data, 1 );

    //Set normal display
    data[0] = 0xA6;
    SendCommand( data, 1 );

    // Set display on
    data[0] = 0xAF;
    SendCommand( data, 1 );
}


/*----------------------------------------------------------------------------*/
void SSD1306_UpdateScreen()
{
    SendData( pixelBuffer, SSD1306_BUFFER_SIZE );
}


/*----------------------------------------------------------------------------*/
void SSD1306_ClearScreen()
{
    for ( uint16_t i = 0; i < SSD1306_BUFFER_SIZE; i++ )
    {
        pixelBuffer[i] = 0x00;
    }

    SSD1306_UpdateScreen();
}


/*----------------------------------------------------------------------------*/
static void SetPixel( uint8_t x, uint8_t y )
{
    pixelBuffer[x + ( y / 8 ) * SSD1306_X_SIZE] |= ( 1 << ( y % 8 ) );
}


/*----------------------------------------------------------------------------*/
void SSD1306_DrawFilledRect( uint8_t xStart, uint8_t xEnd, uint8_t yStart, uint8_t yEnd )
{
    for ( uint8_t i = xStart; i < xEnd; i++ )
    {
        for ( uint8_t j = yStart; j < yEnd; j++ )
        {
            SetPixel( i, j );
        }
    }
}


/*----------------------------------------------------------------------------*/
uint8_t SSD1306_IsReady()
{
    return SSD1306_state == SSD1306_BUSY ? HAL_OK : HAL_ERROR;
}


/*----------------------------------------------------------------------------*/
