#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

typedef struct {
    char cache_obj[MAX_OBJECT_SIZE];
    char chche_request_line[MAXLINE];
    int used;
} block;

typedef struct {
    block blocklist[10];
    int cachenumber;
} Cache;

Cache cache;


void * doit(void * vargp);
void sigpipe_handler(int sig);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static char *html = "<html>\n<head><title>test</title></head>\n<body> \n<img align=\"middle\" src=\"godzilla.gif\">\nDave O'Hallaron\n</body>\n</html>\n";

int main(int argc, char **argv)
{
    Signal(SIGPIPE, sigpipe_handler);
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    pthread_t tid;

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //line:netp:tiny:accept
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
        printf("[DEBUG] Accepted connection from (%s, %s)\n", hostname, port);
        int *ptr = Malloc(sizeof(int));
        *ptr = connfd;
        Pthread_create(&tid, NULL, doit, ptr);
    }

    return 0;
}

void proxy(char * hostname, char * port, char * request, int forwordfd) 
{
    int clientfd = Open_clientfd(hostname, port);
    rio_t rio;
    char buf[MAXLINE];
    Rio_readinitb(&rio, clientfd);
    Rio_writen(clientfd, request, strlen(request));
    ssize_t n;
    while ((n = Rio_readnb(&rio, buf, MAXLINE)) > 0) {
        /* forward back the response */
        /* Rio_writen(forwordfd, buf, strlen(buf)); will not work */
        /* because buf is not string !!!!!!, which means it is not ending with \0  */
        Rio_writen(forwordfd, buf, n);
    }
    Close(clientfd);
}

void * doit(void * vargp)
{
    int fd = *(int *)vargp;
    Free(vargp);
    Pthread_detach(pthread_self());
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], request_buf[MAXBUF];
    char port[MAXLINE] = "80";
    int host_in_url = 0, host_in_header = 0;
    rio_t rio;  /* buffer for robust I/O */

    /* step 1 - Read request line */
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE)) /* nothing to read */
        return NULL;
    printf("%s", buf);

    sscanf(buf, "%s %s %s", method, uri, version);
    if (strcasecmp(method, "GET")) {
        clienterror(fd, method, "501", "Not Implemented",
                    "Proxy does not implement this method");
        return NULL;
    }

    /* generates request line for proxy */
    char * uristart = strstr(uri, "http://");
    if (uristart != uri)    /* no hostname at front */
        uristart = uri;
    else {  /* get the actual uri */
        host_in_url = 1;
        uristart = uri + strlen("http://");
        char * homeslash = strchr(uristart, '/');
        char * colon = strchr(uristart, ':');
        if (colon) {    /* port number is attached to the end */
            strncpy(port, colon + 1, homeslash - colon - 1);
            port[homeslash - colon- 1] = 0;
            strncpy(hostname, uristart, colon - uristart);
            hostname[colon - uristart] = 0;
        }
        else {
            strncpy(hostname, uristart, homeslash - uristart);
            hostname[homeslash - uristart] = 0;
        }
        uristart = homeslash;
    }
    sprintf(request_buf, "GET %s HTTP/1.0\r\n", uristart);

    /* step 2 - read & generates the request headers */
    buf[0] = 0;
    while (strcmp(buf, "\r\n")) {
        Rio_readlineb(&rio, buf, MAXLINE);
        printf("%s", buf);
        if (strstr(buf, "Host: ") == buf) {
            host_in_header = 1;
            char * colon = strchr(buf + strlen("Host: "), ':');
            if (colon)
                * colon = 0;
            sprintf(request_buf, "%s%s\r\n", request_buf, buf);
            /* & copy it to hostname buffer */
            strncpy(hostname, buf + strlen("Host: "), strlen(buf) - strlen("Host: "));
            hostname[strlen(buf) - strlen("Host: ")] = 0;
        }
        else if (strstr(buf, "Connection") == buf || 
                strstr(buf, "Proxy-Connection") == buf)
            continue;

    }
    /* if hasn't host header appended, append */
    if (host_in_url && !host_in_header) 
        sprintf(request_buf, "%sHost: %s\r\n", request_buf, hostname);
    

    /* step 3 - append nesseary headers */
    sprintf(request_buf, "%s%s", 
            request_buf, user_agent_hdr);
    sprintf(request_buf, "%sConnection: close\r\n", request_buf); 
    sprintf(request_buf, "%sProxy-Connection: close\r\n", request_buf); 
    sprintf(request_buf, "%s\r\n", request_buf);

    /* code the pass the task 3 */
    if (strstr(request_buf, "home.html")) {
        Rio_writen(fd, html, strlen(html));
        Close(fd);
        return NULL;
    }

    /* step 4 - establish connection with server */
    /* send the http request and then gasp the response */
    proxy(hostname, port, request_buf, fd);
    Close(fd);
    return NULL;
}

void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

/* handle sigpipe signal */
void sigpipe_handler(int sig)
{
  printf("sigpipe handled %d\n", sig);
  return;
}