#include "rio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    char buf[BUFSIZE];
    char str[] = "this is what i write\n";
    int fd = open("./test", O_RDWR | O_CREAT);
    rio_readn(fd, &buf, BUFSIZE);
    printf("%s", buf);
    rio_writen(fd, str, strlen(str));
    close(fd);
    
    return 0;
}

