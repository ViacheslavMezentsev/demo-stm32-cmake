#include <stdio.h>
#include "stm32f10x.h"

#include "uip.h"
#include "uip_arp.h"

#include "mb.h"
#include "mbutils.h"

#include "tapdev.h"
#include "enc28j60.h"

#include "bsp_spi1.h"
#include "bsp_usart.h"
#include "timer.h"

#define LED1_ON()   GPIO_SetBits( GPIOB, GPIO_Pin_5 )
#define LED1_OFF()  GPIO_ResetBits( GPIOB, GPIO_Pin_5 )

#define LED2_ON()   GPIO_SetBits( GPIOD, GPIO_Pin_6 )
#define LED2_OFF()  GPIO_ResetBits( GPIOD, GPIO_Pin_6 )

#define LED3_ON()   GPIO_SetBits( GPIOD, GPIO_Pin_3 )
#define LED3_OFF()  GPIO_ResetBits( GPIOD, GPIO_Pin_3 )

#define REG_INPUT_START       0x0000        // Input register start address
#define REG_INPUT_NREGS       16            // Number of input registers

#define REG_HOLDING_START     0x0000        // Holding register start address
#define REG_HOLDING_NREGS     16            // Number of holding registers

#define REG_COILS_START       0x0000        // Coil start address
#define REG_COILS_SIZE        16            // Number of coils

#define REG_DISCRETE_START    0x0000        // Switch register start address
#define REG_DISCRETE_SIZE     16            // Number of switch registers

// input register content
uint16_t usRegInputBuf[ REG_INPUT_NREGS ] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

// register start address
uint16_t usRegInputStart = REG_INPUT_START;

// holding register contents
uint16_t usRegHoldingBuf[ REG_HOLDING_NREGS ] = { 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

// Holding register start address
uint16_t usRegHoldingStart = REG_HOLDING_START;

// Coil Status
uint8_t ucRegCoilsBuf[ REG_COILS_SIZE / 8 ] = { 0xFF, 0x00 };

// switch status
uint8_t ucRegDiscreteBuf[ REG_DISCRETE_SIZE / 8 ] = { 0x00, 0xFF };

#define BUF ( ( struct uip_eth_hdr * ) & uip_buf[0] )

void GPIO_Config( void );
void led_poll( void );

int main( void )
{
    timer_typedef periodic_timer, arp_timer;
    uip_ipaddr_t ipaddr;

    // Modify the TCP timer to 100ms.
    timer_set( & periodic_timer, CLOCK_SECOND / 10 );
    timer_set( & arp_timer, CLOCK_SECOND * 10 );

    // IO port initialization is mainly to avoid other devices on the SPI bus.
    GPIO_Config();

    // Configure systic as 1ms interrupt.
    timer_config();

    // Initialize SPI1.
    BSP_ConfigSPI1();

    // ENC28J60 initialization.
    tapdev_init();

    // UIP protocol stack initialization.
    uip_init();

    // Set IP address.
    uip_ipaddr( ipaddr, 192, 168, 1, 15 );
    uip_sethostaddr( ipaddr );

    // Set default router IP address.
    uip_ipaddr( ipaddr, 192, 168, 1, 1 );
    uip_setdraddr( ipaddr );

    // Set netmask.
    uip_ipaddr( ipaddr, 255, 255, 255, 0 );
    uip_setnetmask( ipaddr );

    // MODBUS TCP listens on default port 502.
    eMBTCPInit( MB_TCP_PORT_USE_DEFAULT );
    eMBEnable();

    BSP_ConfigUSART1();

    printf( "\r\nuip start!\r\n" );
    printf( "ipaddr:192.168.1.15\r\n" );

    while ( 1 )
    {
        eMBPoll();
        led_poll();

        // Read an IP packet from a network device and return the data length.
        uip_len = tapdev_read();

        // Received data.
        if ( uip_len > 0 )
        {
            // Process IP packets.
            if ( BUF->type == htons( UIP_ETHTYPE_IP ) )
            {
                uip_arp_ipin();
                uip_input();

                if ( uip_len > 0 )
                {
                    uip_arp_out();

                    tapdev_send();
                }
            }
            // Process ARP packets.
            else if ( BUF->type == htons( UIP_ETHTYPE_ARP ) )
            {
                uip_arp_arpin();

                if ( uip_len > 0 )
                {
                    tapdev_send();
                }
            }
        }

        // 0.5 second timer timeout.
        if ( timer_expired( & periodic_timer ) )
        {
            timer_reset( & periodic_timer );

            // GPIOD->ODR ^= GPIO_Pin_3;

            // Handling TCP connections, UIP_CONNS defaults to 10.
            for ( uint8_t i = 0; i < UIP_CONNS; i++ )
            {
                // Handling TCP communication events.
                uip_periodic( i );

                if ( uip_len > 0 )
                {
                    uip_arp_out();

                    tapdev_send();
                }
            }

#if UIP_UDP

            // Handle each UDP connection in turn, UIP_UDP_CONNS defaults to 10.
            for ( uint8_t i = 0; i < UIP_UDP_CONNS; i++ )
            {
                // Handling UDP communication events.
                uip_udp_periodic(i);

                // If the above function call results in data that should be sent out, the global variable uip_len is set to a value > 0.
                if ( uip_len > 0 )
                {
                    uip_arp_out();

                    tapdev_send();
                }
            }

#endif /* UIP_UDP */

            // Periodic ARP processing.
            if ( timer_expired( & arp_timer ) )
            {
                timer_reset( & arp_timer );

                uip_arp_timer();
            }
        }
    }
}


/**
 * Function: general IO port configuration
 */
void GPIO_Config( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
        RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE );

    // Please modify the following content according to the development board,
    // please pay attention to other devices on the SPI bus
    // LED1 control
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init( GPIOB, & GPIO_InitStructure );

    // LED2, LED3 control
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_3;

    GPIO_Init( GPIOD, & GPIO_InitStructure );

    // Other devices mounted on the SPI1 bus
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init( GPIOC, & GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_7;
    GPIO_Init( GPIOB, & GPIO_InitStructure );

    // It is very important to disable other devices on the SPI1 bus
    GPIO_SetBits( GPIOB, GPIO_Pin_7 );  // Touch screen chip XPT2046 SPI chip selection prohibited
    GPIO_SetBits( GPIOB, GPIO_Pin_12 ); // VS1003 SPI chip select disable
    GPIO_SetBits( GPIOC, GPIO_Pin_4 );  // SST25VF016B SPI chip select disable

    // ENC28J60 receive complete interrupt pin, not used in this example
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

    GPIO_Init( GPIOA, & GPIO_InitStructure );
}


eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex;

    // Query if it is in the register range
    // To avoid warning, change to signed integer
    if ( ( ( int16_t ) usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );

        while ( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[ iRegIndex ] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[ iRegIndex ] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}


eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode eStatus = MB_ENOERR;

    if ( ( ( int16_t ) usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        int iRegIndex = ( int )( usAddress - usRegHoldingStart );

        switch ( eMode )
        {
            case MB_REG_READ:
            {
                while ( usNRegs > 0 )
                {
                    *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[ iRegIndex ] >> 8 );
                    *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[ iRegIndex ] & 0xFF );
                    iRegIndex++;
                    usNRegs--;
                }

                break;
            }
            case MB_REG_WRITE:
            {
                while ( usNRegs > 0 )
                {
                    usRegHoldingBuf[ iRegIndex ] = *pucRegBuffer++ << 8;
                    usRegHoldingBuf[ iRegIndex ] |= *pucRegBuffer++;
                    iRegIndex++;
                    usNRegs--;
                }

                break;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}


eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    eMBErrorCode eStatus = MB_ENOERR;
    short iNCoils = ( short ) usNCoils;

    if ( ( ( int16_t ) usAddress >= REG_COILS_START ) && ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
    {
        unsigned short usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );

        switch ( eMode )
        {
            case MB_REG_READ:
            {
                while ( iNCoils > 0 )
                {
                    * pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset, ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ) );

                    iNCoils -= 8;
                    usBitOffset += 8;
                }

                break;
            }
            case MB_REG_WRITE:
            {
                while ( iNCoils > 0 )
                {
                    xMBUtilSetBits( ucRegCoilsBuf, usBitOffset, ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ), * pucRegBuffer++ );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }

                break;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}


eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode eStatus = MB_ENOERR;
    short iNDiscrete = ( short ) usNDiscrete;

    if ( ( ( int16_t ) usAddress >= REG_DISCRETE_START ) && ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
    {
        unsigned short usBitOffset = ( unsigned short )( usAddress - REG_DISCRETE_START );

        while ( iNDiscrete > 0 )
        {
            *pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset, ( unsigned char )( iNDiscrete > 8 ? 8 : iNDiscrete ) );

            iNDiscrete -= 8;
            usBitOffset += 8;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}


// Test LED Control
void led_poll( void )
{
    //uint8_t led_state = ucRegCoilsBuf[0];

    //led_state & 0x01 ? LED1_ON() : LED1_OFF();
    //led_state & 0x02 ? LED2_ON() : LED2_OFF();
    //led_state & 0x04 ? LED3_ON() : LED3_OFF();
}
