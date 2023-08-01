int _close_r( int fd )
{
    (void) fd;
    return -1;
}

int _read_r( int fd, char* ptr, int len )
{
    (void) fd, (void) ptr, (void) len;
    return -1;
}

int _write_r( int fd, char* ptr, int len )
{
    (void) fd, (void) ptr, (void) len;
    return -1;
}

int _lseek_r( int fd, int ptr, int dir )
{
    (void) fd, (void) ptr, (void) dir;
    return 0;
}