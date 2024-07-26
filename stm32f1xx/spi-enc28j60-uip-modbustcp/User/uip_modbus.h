#ifndef __UIP_MODBUS_H
#define __UIP_MODBUS_H

#include "uipopt.h"
#include "psock.h"

/* application state */
typedef int uip_tcp_appstate_t;

/* Define the application and specify the UIP_APPCALL macro definition */
void uip_modbus_appcall( void );

#ifndef UIP_APPCALL
#define UIP_APPCALL uip_modbus_appcall
#endif

void uip_modbus_init( void );
void uip_modbus_appcall( void );

#endif
