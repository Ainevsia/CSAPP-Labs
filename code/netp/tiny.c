/*
 * tiny.c - A simple, iterative HTTP/1.0 Web sever that uses the
 *  GET method to serve static and dynamic content
 * */
#include "rio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    int listenfd, connfd;
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        _exit(1);
    }
    
    return 0;
}
