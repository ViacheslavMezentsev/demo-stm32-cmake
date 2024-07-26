#include "tapdev.h"
#include "uip.h"
#include "uip_arp.h"
#include "enc28j60.h"

struct uip_eth_addr uip_mac;

// MAC address.
static unsigned char ethernet_mac[6] = { 0x04, 0x02, 0x35, 0x00, 0x00, 0x01 };

/// Configure the network card hardware, set the IP address.
void tapdev_init( void )
{
    unsigned char i;

    // Initialize enc28j60.
    enc28j60_init( ethernet_mac );

    for ( i = 0; i < 6; i++ )
    {
        uip_mac.addr[i] = ethernet_mac[i];
    }

    // Set mac address.
    uip_setethaddr( uip_mac );
}


/**
 * entry parameter: read a packet of data
 * export parameters: Returns the packet length, in bytes, if a packet is received, otherwise zero.
 */
uint16_t tapdev_read( void )
{
    return enc28j60_packet_receive( uip_buf, 1500 );
}


/**
 * entry parameter: send a packet of data
 * export parameters:
 */
void tapdev_send( void )
{
    enc28j60_packet_send( uip_buf, uip_len );
}
