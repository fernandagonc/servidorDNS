
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "funcoes_servidor.h"
#include "common.h"
#define PROTOCOLO "v4"
#define SIZE 1024

void printAddr(const struct sockaddr *addr, char *str, size_t strsize) {
    int version;
    char addrstr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;

    if (addr->sa_family == AF_INET) {
        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr,
                       INET6_ADDRSTRLEN + 1)) {
            printf("ntop");
        }
        port = ntohs(addr4->sin_port); // network to host short
    } else if (addr->sa_family == AF_INET6) {
        version = 6;
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrstr,
                       INET6_ADDRSTRLEN + 1)) {
            printf("ntop");
        }
        port = ntohs(addr6->sin6_port); // network to host short
    } else {
        printf("unknown protocol family.");
    }
    if (str) {
        snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
    }
}


int addrParse(const char *addrstr, int port, struct sockaddr_storage *storage) {
    if (addrstr == NULL) {
        return -1;
    }

    uint16_t porta = htons(port);

    struct in_addr inaddr4;
    if (inet_pton(AF_INET, addrstr, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = porta;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6;
    if (inet_pton(AF_INET6, addrstr, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = porta;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

int inicializarSocketAddr(const char *proto, const char *portstr, struct sockaddr_storage *storage, char * ip) {
   
    uint16_t port = (uint16_t)atoi(portstr); 
    if (port == 0) {
        return -1;
    }
    port = htons(port); 

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4")) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        inet_pton(AF_INET6, ip, &addr4);
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY;
        addr4->sin_port = port;
        return 0;
    } else if (0 == strcmp(proto, "v6")) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        inet_pton(AF_INET6, ip, &addr6);
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any;
        addr6->sin6_port = port;
        return 0;
    } else {
        return -1;
    }
};

int criarSocket(char * porta, char * ip){
    struct sockaddr_storage storage;
    if (0 != inicializarSocketAddr(PROTOCOLO, porta, &storage, ip)) {
        exit(1);
    }

	int sockfd;
    sockfd = socket(storage.ss_family, SOCK_DGRAM, 0);
    
	if((sockfd) < 0){
        printf("\n Erro : Não foi possível criar o socket \n");
        exit(1);
    } 
    
    int enable = 1;
    

    if (0 != setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable))) {
        printf("Erro na função setsockopt");
        exit(1);
    }
    
    if (0 != setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable))) {
        printf("Erro na função setsockopt");
        exit(1);
    }

    struct timeval timeout={2,0}; //set timeout for 2 seconds

    if (0 != setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval))) {
        printf("Erro na função setsockopt");
        exit(1);
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(sockfd, addr, sizeof(storage))) {
        printf("Erro no bind");
        exit(1);
    }
    char str[SIZE];
    printAddr(addr,  str,  SIZE);
    printf("Addr: %s\n", str);


    printf("Socket para comunicação criado com sucesso. \n");
    return sockfd;
}