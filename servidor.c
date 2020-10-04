
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "funcoes_dns.h"

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

int charToInt (char value){
    return value - '0';
};


int addrParse(const char *ip, int port, struct sockaddr_storage *storage) {
    if (ip == NULL) {
        return -1;
    }

    uint16_t porta = htons(port);

    struct in_addr inaddr4;
    if (inet_pton(AF_INET, ip, &inaddr4)) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = porta;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6;
    if (inet_pton(AF_INET6, ip, &inaddr6)) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = porta;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

int criarSocket(char * porta, char * ip){
    struct sockaddr_storage storage;
    if (0 != addrParse(ip, atoi(porta), &storage)) {
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

    struct timeval timeout={2,0}; 

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
  
    printf("Socket para comunicação criado com sucesso no endereço: %s\n", str);
    return sockfd;
}