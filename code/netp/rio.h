#ifndef __RIO_C__
#define __RIO_C__

#include "rio.c"

ssize_t rio_readn(int fd, void * userbuf, size_t n);
ssize_t rio_writen(int fd, void * userbuf, size_t n);
void rio_readinitb(rio_t * rp, int fd);
static ssize_t rio_read(rio_t * rp, char * userbuf, size_t n);
ssize_t rio_readlineb(rio_t * rp, void * userbuf, size_t maxlen);
ssize_t rio_readnb(rio_t * rp, void * userbuf, size_t n);
#endif
