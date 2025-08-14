/*
 * syscalls.c
 *
 *  Created on: 16.05.2014
 *      Author: Florian
 */


/*---------------------------------------------------------------------*
 *  include files                                                      *
 *---------------------------------------------------------------------*/

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/*---------------------------------------------------------------------*
 *  local definitions                                                  *
 *---------------------------------------------------------------------*/

#define HEAP_SIZE       0x00700000 // 7 MB

/*---------------------------------------------------------------------*
 *  external declarations                                              *
 *---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*
 *  public data                                                        *
 *---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*
 *  private data                                                       *
 *---------------------------------------------------------------------*/

/* heap */
__attribute__ ((section(".sdram")))

/*---------------------------------------------------------------------*
 *  private functions                                                  *
 *---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*
 *  public functions                                                   *
 *---------------------------------------------------------------------*/

int _read_r (struct _reent* r, int file, char* ptr, int len)
{
    errno = EINVAL;
    return -1;
}

int _lseek_r (struct _reent* r, int file, int ptr, int dir)
{
    return 0;
}

int _write_r (struct _reent* r, int file, char* ptr, int len)
{
    return 0;
}

int _close_r (struct _reent* r, int file)
{
    return 0;
}

caddr_t _sbrk_r (struct _reent* r, int incr)
{
    return (caddr_t)0;
}

int _fstat_r (struct _reent* r, int file, struct stat* st)
{
    st->st_mode = S_IFCHR;

    return 0;
}

int _isatty_r (struct _reent* r, int fd)
{
    return 1;
}

void _exit (int rc)
{
    while (1)
    {
    }
}

int _kill (int pid, int sig)
{
    errno = EINVAL;

    return -1;
}

int _getpid ()
{
    return 1;
}

/*---------------------------------------------------------------------*
 *  eof                                                                *
 *---------------------------------------------------------------------*/
