// TCP SERVER

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "helper.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#define MAX_LINE (1000)

int ParseCmdLine(int argc, char *argv[], char **szPort);

int main(int argc, char **argv)
{
    int list_s;                         // listening socket
    int conn_s;                         // connection socket
    short int port;                     // port number
    struct sockaddr_in servaddr;        // socket address structure
    struct sockaddr_in their_addr;      
    char buffer[MAX_LINE];              // character buffer
    char *endptr;                       // for strtol()
    int sin_size;

    ParseCmdLine(argc, argv, &endptr);

    port = strtol(endptr, &endptr, 0);
    if(*endptr) {
        fprintf(stderr, "server: porta non riconosciuta\n");
        exit(-1);
    }

    fprintf("Server in ascolto sulla porta %d\n", port);

    if((list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "server: errore nella creazione della socket\n");
        exit(-1);
    }


    // set all bytes in socket structure to zero, and fill in the relevant data members

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // bind our socket address to the listening socket, and call listen()

    if(bind(list_s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "server: errore durante la bind\n");
        exit(-1);
    }

    if(listen(list_s, LISTENQ) < 0) {
        fprintf(stderr, "server: errore durante la listen\n");
        exit(-1);
    }

    // enter an infinite loop to respond to client request and echo input

    while(1) {
        // wait for a connection, then accept() it
        sin_size = sizeof(struct sockaddr_in);
        if((conn_s = accept(list_s, (struct sockaddr *)&their_addr, &sin_size)) < 0) {
            fprintf(stderr, "server: errore nella accept\n");
            exit(-1);
        }
        printf("server: connessione da %s\n", inet_ntoa(their_addr.sin_addr));

        // retrieve an input line from the connected socket then simply it back to the same socket

        Readline(conn_s, buffer, MAX_LINE-1);
        printf("server: il client ha scritto:\n\t%s\n", buffer);
        Writeline(conn_s, buffer, strlen(buffer));

        // close the connected socket

        if(close(conn_s) < 0) {
            fprintf(stderr, "server: errore durante la close\n");
            exit(-1);
        }
    }

    if(close(list_s) < 0) {
        fprintf(stderr, "server: errore durante la close\n");
        exit(-1);
    }
}

int ParseCmdLine(int argc, char **argv, char **szPort)
{
    int n = 1;

    while(n < argc) {
        if(!strncmp(argv[n], "-p", 2) || !strncmp(argv[n], "-P", 2)) {
            *szPort = argv[++n];
        } else {
            if(!strncmp(argv[n], "-h", 2) || !strncmp(argv[n], "-H", 2)) {
                printf("Sintassi:\n\n");
                printf("    server -p (porta) [-h]\n\n");
                exit(-1);
            }
        }
        ++n;
    }

    if(argc == 1){
        printf("Sintassi:\n\n");
        printf("    server -p (porta) [-h]\n\n");
        exit(-1);
    }

    return 0;
}