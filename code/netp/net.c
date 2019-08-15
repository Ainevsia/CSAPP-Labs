#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int open_listenfd(char * port)
{
    struct addrinfo hints, * listp, * p;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;
    int rc = getaddrinfo(NULL, port, &hints, &listp);
    if (rc != 0) {
        fprintf(stderr, "error: %s\n", gai_strerror(rc));
        exit(1);
    }

    /* walk the list and display each ip address */
    for (p = listp; p; p = p->ai_next) {
        /* (struct addrinfo *)p & inet_ntop receive the src as pointer to in_addr */
        inet_ntop(AF_INET, &(((struct sockaddr_in *)(p->ai_addr))->sin_addr), buf, MAXLINE);
        printf("%s\n", buf);
    }
}