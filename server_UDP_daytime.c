#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SERV_PORT 5193
#define MAXLINE 1024

int main(int argc, char **argv)
{
    int sockfd, len;
    struct sockaddr_in addr;
    char buff[MAXLINE];
    time_t ticks;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("errore in socket");
        exit(-1);
    }

    memset((void *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERV_PORT);

    if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("errore in bind");
        exit(-1);
    }

    while(1) {
        len = sizeof(addr);
        if((recvfrom(sockfd, buff, MAXLINE, 0, (struct sockaddr *)&addr, &len)) < 0) {
            perror("errore in recvfrom");
            exit(-1);
        }

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));

        if(sendto(sockfd, buff, strlen(buff), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("errore in sendto");
            exit(-1);
        }
    }

    exit(0);
}