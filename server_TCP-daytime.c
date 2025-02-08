#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SERV_PORT 5193
#define BACKLOG 10
#define MAXLINE 1024

int main(int argc, char **argv)
{
    int listensd, connsd;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    time_t ticks;

    if((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        perror("errore in socket");
        exit(-1);
    }

    memset((void *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if(bind(listensd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("errore in bind");
        exit(-1);
    }

    if(listen(listensd, BACKLOG) < 0) {
        perror("errore in listen");
        exit(-1);
    }

    while(1) {
        if((connsd = accept(listensd, (struct sockaddr *) NULL, NULL)) < 0) {
            perror("errore in accept");
            exit(-1);
        }

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%24s\r\n", ctime(&ticks));

        if(write(connsd, buff, strlen(buff)) != strlen(buff)) {
            perror("errore in write");
            exit(-1);
        }

        if(close(connsd) == -1){
            perror("errore in close");
            exit(-1);
        }
    }

    exit(0);
}