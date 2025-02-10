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

void str_cli_echo(FILE *fd, int sockd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    int n;

    while(fgets(sendline, MAXLINE, fd) != NULL) {
        if((n = writen(sockd, sendline, strlen(sendline))) < 0) {
            perror("errore in write");
            exit(1);
        }

        if(( n = readline(sockd, recvline, MAXLINE)) < 0) {
            fprintf(stderr, "errore in readline");
            exit(1);
        }

        fputs(recvline, stdout);
    }
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;

    if(argc != 2) {
        fprintf(stderr, "utilizzo: echo_client <indirizzo IP server>\n");
        exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("errore in socket");
        exit(1);
    }

    memset((void *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);

    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        fprintf(stderr, "errore in inet_pton per %s", argv[1]);
        exit(1);
    }

    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("errore in connect");
        exit(1);
    }

    str_cli_echo(stdin, sockfd);

    close(sockfd);

    exit(0);
}