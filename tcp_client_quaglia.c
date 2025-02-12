// TCP CLIENT

#include <sys/socket.h>       
#include <sys/types.h>      
#include <arpa/inet.h>       
#include <unistd.h>           
#include <netinet/in.h>
#include <netdb.h>

#include "helper.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE (1000)

int ParseCmdLine(int , char **, char **, char **);

int main(int argc, char **argv)
 {
    int conn_s;                     // connection socket
    short int port;                 // port number
    struct sockaddr_in servaddr;    // socket address structure
    char buffer[MAX_LINE];          // character buffer
    char *szAddress;                // holds remote IP address
    char *szPort;                   // holds remote port
    char *endptr;                   // for strtol()
    struct hostent *he;             

    he = NULL;

    // get command line arguments

    ParseCmdLine(argc, argv, &szAddress, &szPort);

    // set the remote port

    port = strtol(szPort, &endptr, 0);
    if(*endptr) {
        printf("client: porta non riconosciuta\n");
        exit(1);
    }

    // create the listening socket

    if((conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "client: errore durante la creazione della socket\n");
        exit(1);
    }

    // set all bytes in socket address structure to zero, and fill in the relevant data members

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    // set the remote IP address
    
    if(inet_aton(szAddress, &servaddr.sin_addr) <= 0) {
        printf("client: indirizzo IP non valido\nclient: risoluzione nome... ");
        if((het = gethostbyname(szAddress)) == NULL) {
            printf("fallita\n");
            exit(1);
        }
        printf("riuscita\n\n");
        servaddr.sin_addr = *((struct in_addr *)he->h_addr);
    }

    // connect to the remote echo server

    if(connect(conn_s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("client: errore durante la connect\n");
        exit(1);
    }

    // get string to echo from user

    printf("Inserire la stringa da spedire: ");
    fgets(buffer, MAX_LINE, stdin);

    // send string to echo server, and retrieve response

    Writeline(conn_s, buffer, strlen(buffer));
    memset(buffer, 0, sizeof(char)*(strlen(buffer)+1));
    Readline(conn_s, buffer, MAX_LINE-1);

    // output echoed string

    printf("Risposta dal server: %s\n", buffer);

    return 0;
 }

int ParseCmdLine(int argc, char **argv, char **szPort)
{
    int n = 1;

    while(n < argc) {
        if(!strncmp(argv[n], "-a", 2) || !strncmp(argv[n], "-A", 2)) {
            *szAddress = argv[++n];
        } else {
            if(!strncmp(argv[n], "-p", 2) || !strncmp(argv[n], "-P", 2)) {
                *szPort = argv[++n];
            } else {
                if(!strncmp(argv[n], "-h", 2) || !strncmp(argv[n], "-H", 2)) {
                    printf("Sintassi:\n\n");
                    printf("    client -a (indirizzo server) -p (porta del server) [-h]\n\n");
                    exit(-1);
                }
            }  
        }
        ++n;
    }

    if(argc == 1){
        printf("Sintassi:\n\n");
        printf("    client -a (indirizzo server) -p (porta del server) [-h]\n\n");
        exit(-1);
    }

    return 0;
}