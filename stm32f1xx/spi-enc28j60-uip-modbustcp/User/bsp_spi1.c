/* reference header file *****************************************************************/
#include "stm32f10x.h"
#include "bsp_spi1.h"
/* external data type ***************************************************************/
/* external constant macro *****************************************************************/
/* External Action Macro *****************************************************************/
/* external variable *******************************************************************/
/* External function declaration ***************************************************************/

/*
********************************************************************************
* Function name: BSP_ConfigSPI1
* Function description: SPI1 related configuration operations, including SPI1 mode, SPI1 communication speed and related IO ports
* Parameters: none
* Return value: None
* Instructions for use: called during BSP initialization
* Calling method: BSP_ConfigSPI1();
********************************************************************************
*/
void BSP_ConfigSPI1( void )
{
    /* GPIO structure */
    GPIO_InitTypeDef  GPIO_InitStructure;
    /* SPI structure */
    SPI_InitTypeDef SPI_InitStructure;

    /* Enable related clocks on APB2 */
    /* Enable SPI clock, enable GPIOA clock */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE );

    /* SPI1 SCK@GPIOA.5 SPI1 MOSI@GPIOA.7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* Multiplexed push-pull output */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOA, & GPIO_InitStructure );

    /* SPI1 MISO@GPIOA.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* floating input */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOA, & GPIO_InitStructure );

    /* 2-wire bi-directional full duplex */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;

    /* host mode */
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

    /* 8-bit frame structure */
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;

    /* Low when the clock is idle */
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;

    /* The first rising edge captures data */
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;

    /* MSS Port software control */
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

    /* SPI clock 72Mhz / 8 = 9M */
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;

    /* Data transfer high order first */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;

    SPI_InitStructure.SPI_CRCPolynomial = 7;

    /* Initialize SPI1 */
    SPI_Init( SPI1, & SPI_InitStructure );

    /* Enable the SS output function of the SPI port GPIOA.4 */
    SPI_SSOutputCmd( SPI1, ENABLE );

    /* Enable SPI1 */
    SPI_Cmd( SPI1, ENABLE );
}


/*
********************************************************************************
* Function name: BSP_SPI1SendByte
* Function description: SPI1 sends byte data
* Parameter: uint8_t byte send byte
* Return value: uint8_t return byte
* Instructions for use: According to the SPI1 communication principle, there must be a return byte when sending bytes
* Calling method: BSP_SPI1SendByte(value);
********************************************************************************
*/
uint8_t BSP_SPI1SendByte( uint8_t byte )
{
    /* Waiting for the send buffer register to be empty */
    while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) == RESET );

    /* send data */
    SPI_I2S_SendData( SPI1, byte );

    /* Wait for the receive buffer register to be non-empty */
    while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_RXNE ) == RESET );

    return SPI_I2S_ReceiveData( SPI1 );
}


/*
********************************************************************************
* Function name: BSP_SPI1ReceiveByte
* Function description: SPI1 receives byte data
* Parameters: none
* Return value: uint8_t return byte
* Instructions for use: According to the SPI1 communication principle, there
* must be a return byte when sending bytes. This function is the same as the sending function.
* Calling method: value = BBSP_SPI1ReceiveByte();
********************************************************************************
*/
uint8_t BSP_SPI1ReceiveByte( void )
{
    /* Waiting for the send buffer register to be empty */
    while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_TXE ) == RESET );

    /* Send data, get return data by sending 0xFF */
    SPI_I2S_SendData( SPI1, 0xFF );

    /* Wait for the receive buffer register to be non-empty */
    while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_RXNE ) == RESET );

    /* Returns the data received from the SPI communication */
    return SPI_I2S_ReceiveData( SPI1 );
}
