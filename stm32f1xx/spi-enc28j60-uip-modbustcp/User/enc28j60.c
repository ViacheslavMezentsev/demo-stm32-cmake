#include "enc28j60.h"
#include "uip.h"
#include "bsp_spi1.h"

/* Bank number ENC28J60 has 4 banks from Bank0 to Bank3 Need to be selected by ECON1 register */
static unsigned char enc28j60_bank;

/* Next data packet pointer, see data sheet P43 Figure 7-3 */
static unsigned int next_pack_ptr;

#define ENC28J60_CSL()      GPIO_ResetBits( GPIOA, GPIO_Pin_4 )
#define ENC28J60_CSH()      GPIO_SetBits( GPIOA, GPIO_Pin_4 )

/**
* Function description: read register command
* Parameters: unsigned char op opcode
*           unsigned char address register address
* Return value: return data
* Instructions for use: Opcodes supported by this function are read-only control registers, read buffers
* Calling method: enc28j60_readop
*/
unsigned char enc28j60_readop( unsigned char op, unsigned char address )
{
    // CS is pulled low to enable ENC28J60.
    ENC28J60_CSL();

    // opcodes and addresses.
    unsigned char dat = op | ( address & ADDR_MASK );

    // Write data via SPI.
    BSP_SPI1SendByte( dat );

    // Read data through SPI.
    dat = BSP_SPI1SendByte( 0xFF );

    // In the case of MAC and MII registers, the first byte read is invalid,
    // the information is contained in the MSB of the address.
    if ( address & 0x80 )
    {
        // Read data again via SPI.
        dat = BSP_SPI1SendByte( 0xFF );
    }

    // CS is pulled high to disable ENC28J60.
    ENC28J60_CSH();

    // return data.
    return dat;
}


/**
* Function description: write register command
* Parameters: unsigned char op opcode
*           unsigned char address register address
*           unsigned char data      data input
* Instructions for use: The opcodes supported by this function are: write control register bit field clear bit field set 1
* Calling method: enc28j60_writeop
*/
void enc28j60_writeop( unsigned char op, unsigned char address, unsigned char data )
{
    // Enable ENC28J60.
    ENC28J60_CSL();

    // Send opcode and register address via SPI.
    unsigned char dat = op | ( address & ADDR_MASK );

    // Send data through SPI1.
    BSP_SPI1SendByte( dat );

    // prepare register value.
    dat = data;

    // Send data via SPI.
    BSP_SPI1SendByte( dat );

    // Ban ENC28J60.
    ENC28J60_CSH();
}


/**
* Function Description: read buffer
* Parameters: unsigned int len        read length
*           unsigned char* data     read pointer
*/
void enc28j60_readBuffer( unsigned char * pdata, unsigned int len )
{
    // Enable ENC28J60.
    ENC28J60_CSL();

    // Send read buffer command via SPI.
    BSP_SPI1SendByte( ENC28J60_READ_BUF_MEM );

    // cyclic read.
    while ( len )
    {
        len--;

        // read data.
        * pdata = ( unsigned char ) BSP_SPI1SendByte(0);

        // address pointer accumulation.
        pdata++;
    }

    // Ban ENC28J60.
    ENC28J60_CSH();
}


/**
* Function Description: write buffer
* Parameters: unsigned int len        read length
*           unsigned char* data     read pointer
*/
void enc28j60_writebuf( unsigned char * pdata, unsigned int len )
{
    // Enable ENC28J60.
    ENC28J60_CSL();

    // Send write buffer command via SPI.
    BSP_SPI1SendByte( ENC28J60_WRITE_BUF_MEM );

    // cyclic send.
    while ( len )
    {
        len--;

        // send data.
        BSP_SPI1SendByte( *pdata );

        // address pointer accumulation.
        pdata++;
    }

    // Ban ENC28J60.
    ENC28J60_CSH();
}


/**
* Function name: enc28j60_setbank
* Function Description: Setting the register storage area
* Parameters: unsigned char address register address
*/
void enc28j60_setbank( unsigned char address )
{
    /* Calculate the location of this register address in the access area */
    if ( ( address & BANK_MASK ) != enc28j60_bank )
    {
        /* Clear BSEL1 BSEL0 of ECON1 See data sheet page 15 for details */
        enc28j60_writeop( ENC28J60_BIT_FIELD_CLR, ECON1, ( ECON1_BSEL1 | ECON1_BSEL0 ) );

        /* Please pay attention to the macro definition of the register address, the location of the bit6 bit5 code register storage area */
        enc28j60_writeop( ENC28J60_BIT_FIELD_SET, ECON1, ( address & BANK_MASK ) >> 5 );

        /* Redetermine the current register storage area */
        enc28j60_bank = ( address & BANK_MASK );
    }
}


/**
* Function Description: read register value
* Parameters: unsigned char address   register address
* return value:                         register value
*/
unsigned char enc28j60_read( unsigned char address )
{
    /* Set the register address area */
    enc28j60_setbank( address );

    /* Read register value Send read register command and address */
    return enc28j60_readop( ENC28J60_READ_CTRL_REG, address );
}


/**
* Function Description: write register
* Parameters: unsigned char address   register address
*           unsigned char data      register value
*/
void enc28j60_write( unsigned char address, unsigned char data )
{
    /* Set the register address area */
    enc28j60_setbank( address );

    /* Write Register Value Send Write Register Command and Address */
    enc28j60_writeop( ENC28J60_WRITE_CTRL_REG, address, data );
}


/**
* Function description: write physical register
* Parameters: unsigned char address   physical register address
*           unsigned int data       Physical register values Physical registers are all 16 bits wide
* Instructions for use: PHY registers are not directly accessible through SPI commands, but through a special set of registers
*           See page 19 of the data sheet for details
*/
void enc28j60_writephy( unsigned char address, unsigned int data )
{
    /* Write address to MIREGADR See data sheet page 19 for details */
    enc28j60_write( MIREGADR, address );

    /* Write lower 8 bits of data */
    enc28j60_write ( MIWRL, data );

    /* Write high 8-bit data */
    enc28j60_write( MIWRH, data >> 8 );

    /* Wait for the PHY register write to complete */
    while ( enc28j60_read( MISTAT ) & MISTAT_BUSY );
}


/**
* Function description: Initialize enc28j60
* Parameters: unsigned char* mac_addr  mac address parameter pointer
* return value: none
*/
void enc28j60_init( unsigned char * mac_addr )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Turn on the GPIOA clock */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE );

    /* enc28j60 CS @GPIOA.4 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_Init( GPIOA, & GPIO_InitStructure );

    /* Ban ENC28J60 */
    ENC28J60_CSH();

    /* ENC28J60 software reset This function can be improved */
    enc28j60_writeop( ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET );

    /* Query the ESTAT.CLKRDY bit */
    while ( !( enc28j60_read( ESTAT ) & ESTAT_CLKRDY ) );

    /* Set the start address of the receive buffer This variable is used to keep the
    start address of the next packet each time the buffer is read */
    next_pack_ptr = RXSTART_INIT;

    /* Set receive buffer start pointer */
    enc28j60_write( ERXSTL, RXSTART_INIT & 0xFF );
    enc28j60_write( ERXSTH, RXSTART_INIT >> 8 );

    /* Set receive buffer read pointer */
    enc28j60_write( ERXRDPTL, RXSTART_INIT & 0xFF );
    enc28j60_write( ERXRDPTH, RXSTART_INIT >> 8 );

    /* Set receive buffer end pointer */
    enc28j60_write( ERXNDL, RXSTOP_INIT & 0xFF );
    enc28j60_write( ERXNDH, RXSTOP_INIT >> 8 );

    /* Set send buffer start pointer */
    enc28j60_write( ETXSTL, TXSTART_INIT & 0xFF );
    enc28j60_write( ETXSTH, TXSTART_INIT >> 8 );

    /* Set send buffer end pointer */
    enc28j60_write( ETXNDL, TXSTOP_INIT & 0xFF );
    enc28j60_write( ETXNDH, TXSTOP_INIT >> 8 );

    /* Enable Unicast Filter Enable CRC Check Enable Format Match Auto Filter */
    enc28j60_write( ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN );
    enc28j60_write( EPMM0, 0x3f );
    enc28j60_write( EPMM1, 0x30 );
    enc28j60_write( EPMCSL, 0xf9 );
    enc28j60_write( EPMCSH, 0xf7 );

    /* Enable MAC reception Allow MAC to send pause control frames Stop sending when a pause control frame is received */
    /* Data sheet 34 pages */
    enc28j60_write( MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS );

    /* exit reset state */
    enc28j60_write( MACON2, 0x00 );

    /* Fill all short frames with 0 to 60 bytes long and append a CRC Send CRC enable Frame length check enable MAC full duplex enable */
    /* Tip Since ENC28J60 does not support 802.3 auto-negotiation mechanism, the network card of the opposite end needs to be forced to be set to full-duplex */
    enc28j60_writeop( ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX );

    /* fill in the default value */
    enc28j60_write( MAIPGL, 0x12 );

    /* fill in the default value */
    enc28j60_write( MAIPGH, 0x0C );

    /* fill in the default value */
    enc28j60_write( MABBIPG, 0x15 );

    /* maximum frame length */
    enc28j60_write( MAMXFLL, MAX_FRAMELEN & 0xFF );
    enc28j60_write( MAMXFLH, MAX_FRAMELEN >> 8 );

    /* write MAC address */
    enc28j60_write( MAADR5, mac_addr[0] );
    enc28j60_write( MAADR4, mac_addr[1] );
    enc28j60_write( MAADR3, mac_addr[2] );
    enc28j60_write( MAADR2, mac_addr[3] );
    enc28j60_write( MAADR1, mac_addr[4] );
    enc28j60_write( MAADR0, mac_addr[5] );

    /* Configure PHY to be full duplex LEDB to source current */
    enc28j60_writephy( PHCON1, PHCON1_PDPXMD );

    /* LED status */
    enc28j60_writephy( PHLCON, 0x0476 );

    /* Half-duplex loopback disabled */
    enc28j60_writephy( PHCON2, PHCON2_HDLDIS );

    /* Back to BANK0 */
    enc28j60_setbank( ECON1 );

    /* Enable Interrupt Global Interrupt Receive Interrupt Receive Error Interrupt */
    enc28j60_writeop( ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE | EIE_RXERIE );

    /* receive enable bit */
    enc28j60_writeop( ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN );
}


/**
* Function description: Send data packets through enc28j60
* Parameters: unsigned int len       packet length
*           unsigned char* packet  packet pointer
*/
void enc28j60_packet_send( unsigned char * packet, unsigned int len )
{
    /* Query Transmit Logic Reset Bit */
    while ( ( enc28j60_read( ECON1 ) & ECON1_TXRTS ) != 0 );

    /* Set the start address of the send buffer */
    enc28j60_write( EWRPTL, TXSTART_INIT & 0xFF );
    enc28j60_write( EWRPTH, TXSTART_INIT >> 8 );

    /* Set the end address of the send buffer. This value corresponds to the length of the send data packet */
    enc28j60_write( ETXNDL, ( TXSTART_INIT + len ) & 0xFF );
    enc28j60_write( ETXNDH, ( TXSTART_INIT + len ) >> 8 );

    /* Send control packet format word before sending ???????*/
    enc28j60_writeop( ENC28J60_WRITE_BUF_MEM, 0, 0x00 );

    /* Send packets through ENC28J60 */
    enc28j60_writebuf( packet, len );

    /* start sending */
    enc28j60_writeop( ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS );

    /* Issue with reset send logic. See Rev. B4 Silicon Errata point 12. */
    if ( ( enc28j60_read( EIR ) & EIR_TXERIF ) )
    {
        enc28j60_setbank( ECON1 );
        enc28j60_writeop( ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS );
    }
}


/**
* Function description: Receive data packets through enc28j60
* Parameters: unsigned int maxlen The maximum length of the received data packet
*           unsigned char* packet     packet pointer
* return value: Received packet length
*/
unsigned int enc28j60_packet_receive( unsigned char * packet, unsigned int maxlen )
{
    unsigned int rxstat;
    unsigned int len;

    /* Whether an ethernet packet was received */
    if ( enc28j60_read( EPKTCNT ) == 0 )
    {
        return (0);
    }

    /* Set receive buffer read pointer */
    enc28j60_write( ERDPTL, ( next_pack_ptr ) );
    enc28j60_write( ERDPTH, ( next_pack_ptr ) >> 8 );

    /* Example of Receive Packet Structure Datasheet page 43 */

    /* Read pointer to next packet */
    next_pack_ptr = enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 );
    next_pack_ptr |= enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 ) << 8;

    /* length of read packet */
    len  = enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 );
    len |= enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 ) << 8;

    /* remove CRC count */
    len -= 4;

    /* read receive status */
    rxstat  = enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 );
    rxstat |= enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 ) << 8;

    /* Limit search length */
    if ( len > maxlen - 1 )
    {
        len = maxlen - 1;
    }

    /* Check for CRC and sign errors */
    /* ERXFCON.CRCEN is the default setting. Usually we don't need to check */
    if ( ( rxstat & 0x80 ) == 0 )
    {
        len = 0;
    }
    else
    {
        /* Copy packets from receive buffer */
        enc28j60_readBuffer( packet, len );
    }

    /* Move the receive buffer read pointer */
    enc28j60_write( ERXRDPTL, ( next_pack_ptr ) );
    enc28j60_write( ERXRDPTH, ( next_pack_ptr ) >> 8 );

    /* packet decrement */
    enc28j60_writeop( ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC );

    /* return length */
    return ( len );
}
