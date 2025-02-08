#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERV_PORT 5193
#define MAXLINE 1024

int main(int arcg, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if(argc != 2){
        fprintf(stderr, "utilizzo: daytime_clientTCP <indirizzo IP server>\n");
        exit(-1);
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        perror("errore in socket");
        exit(-1);
    }

    memset((void *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);

    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        perror("errore in inet_pton");
        exit(-1);
    }

    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        perror("errore in connect");
        exit(-1);
    }

    while((n = read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = 0;
        if(fputs(recvline, stdout) == EOF) { 
            perror("errore in fputs");
            exit(-1);
        }
    }

    if(n < 0) {
        perror("errore in read");
        exit(-1);
    }

    exit(0);
}