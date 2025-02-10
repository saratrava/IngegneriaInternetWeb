#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <time.h>

#define SERV_PORT 5193
#define BACKLOG 10
#define MAXLINE 1024


void str_srv_echo(int sockd)
    {
        int nread;
        char line[MAXLINE];

        for(;;) {
            if((nread =readline(sockd, line, MAXLINE)) == 0) {
                return;
            }

            if(writen(sockd, line, nread)) {
                fprintf(stderr, "errore in write");
                exit(1);
            }
        }
    }


int main(int argc, char **argv)
{
    pid_t pid;
    int listensd, connsd;
    struct sockaddr_in servaddr, cliaddr;
    int len;

    if((listensd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("errore in socket");
        exit(-1);
    }

    memset((char*)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    if((bind(listensd, (struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
        perror("errore in bind");
        exit(-1);
    }

    if(listen(listensd, BACKLOG) < 0) {
        perror("errore in listen");
        exit(-1);
    }

    for(;;) {
        len = sizeof(cliaddr);
        if((connsd = accept(listensd, (struct sockaddr *)&cliaddr, &len)) < 0) {
            perror("errore in accept");
            exit(-1);
        }

        if((pid = fork()) == 0) {
            if(close(listensd) == -1){
                perror("errore in close");
                exit(-1);
            }
            printf("%s.%d connesso\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
            str_srv_echo(connsd);

            if(close(connsd) == -1){
                perror("errore in close");
                exit(-1);
            }
            exit(0);
        }

        if(close(connsd) == -1){
            perror("errore in close");
            exit(-1);
        }
    }
}