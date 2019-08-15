#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFSIZE 1024
#define RIO_BUFSIZE 8192

ssize_t rio_readn(int fd, void * userbuf, size_t n)
{
    /* read n bytes from file fd into userbuf robust */
    size_t nleft = n;       /* remaining bytes to be read */
    ssize_t nread = 0;      /* short count : already read bytes */
    char * bufp = userbuf;  /* pointer current pos */

    for (; nleft > 0; nleft -= nread, bufp += nread) {
        nread = read(fd, bufp, nleft);  /* bytes read */
        if (nread < 0) {
            if (errno == EINTR) /* be interrupted */
                continue;
            else                /* read() error */
                return -1;
        }
        else if (nread == 0)    /* EOF */
            break;
    }
    return n - nleft;
}

ssize_t rio_writen(int fd, void * userbuf, size_t n)
{
    /* will always return n */
    size_t nleft = n;       /* remaining bytes to be written */
    ssize_t nwritten = 0;   /* short count : already write bytes */
    char * bufp = userbuf;  /* pointer current pos */

    for (; nleft > 0; nleft -= nwritten, bufp += nwritten) {
        nwritten = write(fd, bufp, nleft);  /* bytes read */
        if (nwritten <= 0) {
            if (errno == EINTR) /* be interrupted */
                continue;
            else                /* write() error */
                return -1;
        }
    }
    return n - nleft;
}

typedef struct {
    int rio_fd;
    int rio_cnt;        /* unread bytes in the buffer */
    char * rio_bufptr;  
    char rio_buf[RIO_BUFSIZE];
} rio_t;    /* ? struct name */


/*
 * b stands for buffer
 * */
void rio_readinitb(rio_t * rp, int fd) 
{
    rp->rio_fd = fd;
    rp->rio_bufptr = rp->rio_buf;   /* points to the start of buffer */
    rp->rio_cnt = 0;
}

/*
 * buffered Linux read() with same behaviour
 * used to replace read(fd, buf, 1) [efficiency]
 * */
static ssize_t rio_read(rio_t * rp, char * userbuf, size_t n)
{
    /* if empty, refill the buf */
    while (rp->rio_cnt <= 0) {
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if (rp->rio_cnt < 0)
            if (errno == EINTR)
                continue;
            else
                return -1;
        else if (rp->rio_cnt == 0 )     /* EoF */
            return 0;
        else    /* read success, set bufptr */
            rp->rio_bufptr = rp->rio_buf;
    }

    int cnt = n;    /* cnt = min(n, rp->rio_cnt) */
    if (rp->rio_cnt < n)
        cnt = rp->rio_cnt;
    memcpy(userbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

ssize_t rio_readlineb(rio_t * rp, void * userbuf, size_t maxlen)
{
    int n, rc;
    char c, * bufp = userbuf;
    /* read maxlen - 1 bytes top */
    for (n = 1; n < maxlen; n++) {
        rc = rio_read(rp, &c, 1);
        if (rc == 1) {
            *bufp++ = c;
            if (c == '\n') {
                n++;
                break;
            }
        }
        else if (rc == 0) {
            /* EOF */
            break;
        }
        else
            return -1;
    }
    if (n != 1) /* if read something */
        *bufp = 0;
    return n - 1;
}

ssize_t rio_readnb(rio_t * rp, void * userbuf, size_t n)
{
    /* read n bytes from file fd into userbuf robust */
    size_t nleft = n;       /* remaining bytes to be read */
    ssize_t nread = 0;      /* short count : already read bytes */
    char * bufp = userbuf;  /* pointer current pos */

    for (; nleft > 0; nleft -= nread, bufp += nread) {
        nread = rio_read(rp, bufp, nleft);  /* bytes read */
        if (nread < 0) {
            /* read() error */
            return -1;
        }
        else if (nread == 0)    /* EOF */
            break;
    }
    return n - nleft;
}