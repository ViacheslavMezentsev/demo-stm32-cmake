 /*
  * FreeModbus Libary: MCF5235 Port
  * Copyright (C) 2006 Christian Walter <wolti@sil.at>
  * Parts of crt0.S Copyright (c) 1995, 1996, 1998 Cygnus Support
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  *
  * File: $Id: port.h,v 1.1 2006/08/30 23:18:07 wolti Exp $
  */

#ifndef _PORT_H
#define _PORT_H

#include <assert.h>

/* ----------------------- Defines ------------------------------------------*/

#define INLINE                  inline
#define PR_BEGIN_EXTERN_C       extern "C" {
#define PR_END_EXTERN_C         }

#ifndef TRUE
#define TRUE                    1
#endif

#ifndef FALSE
#define FALSE                   0
#endif

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif
#define MB_TCP_DEBUG            0       /* Debug output in TCP module. */

#define ENTER_CRITICAL_SECTION( )
#define EXIT_CRITICAL_SECTION( )
/* ----------------------- Type definitions ---------------------------------*/
typedef char    BOOL;

typedef unsigned char UCHAR;
typedef char    CHAR;

typedef unsigned short USHORT;
typedef short   SHORT;

typedef unsigned long ULONG;
typedef long    LONG;

#ifdef MB_TCP_DEBUG
typedef enum
{
    MB_LOG_DEBUG,
    MB_LOG_INFO,
    MB_LOG_WARN,
    MB_LOG_ERROR
} eMBPortLogLevel;
#endif

/* ----------------------- Function prototypes ------------------------------*/
#ifdef MB_TCP_DEBUG
void            vMBPortLog( eMBPortLogLevel eLevel, const CHAR * szModule,
                            const CHAR * szFmt, ... );
#endif

#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
