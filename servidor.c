
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "funcoes_servidor.h"
#define PROTOCOLO "v4"
#define SIZE 1024

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

ServerLinks criarSocket(char * porta, char * ip){
    ServerLinks conexao;
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

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(sockfd, addr, sizeof(storage))) {
        printf("Erro no bind");
        exit(1);
    }
    
    conexao.socket = sockfd;
    conexao.storage = &storage;

    printf("Socket para comunicação criado com sucesso. \n");
    return conexao;
}

void sendMessage(int socket, char * ip, int port, char* message) {
    struct sockaddr_in address; 
    memset(&address, 0, sizeof(address)); 

    struct in_addr inaddr4;
    inet_pton(AF_INET, ip, &inaddr4);

    address.sin_family = AF_INET; 
    address.sin_port = htons(port); 
    address.sin_addr = inaddr4;
    char * requisicao;
    requisicao = malloc(SIZE);
    memset(requisicao, 0, SIZE);
    requisicao[0] = '1';
    strcat(requisicao, message);

    int send = sendto(socket, (const char *)requisicao, strlen(requisicao), MSG_CONFIRM, (const struct sockaddr *) &address, sizeof(address)); 
    if(send < 0){
        perror("Erro no send: ");
    }
    else{
        printf("Enviada com sucesso!\n");
    }
}

void receiveMessage(int socket, struct sockaddr *address) {
    char buffer[SIZE]; 
    struct sockaddr_in cliaddr; 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
    socklen_t len = sizeof(cliaddr);
    int n = recvfrom(socket, (char *)buffer, SIZE, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len); 
    buffer[n] = '\0'; 
    printf("recvd");

    printf("%s", buffer);
}