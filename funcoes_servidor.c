#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <arpa/inet.h>
#include "funcoes_servidor.h"
#define SIZE 1024

struct HostnameIP novaEntrada(char* hostname, char *enderecoIP){
    HostnameIP novaEntrada;

    memcpy(novaEntrada.hostname, hostname, 50);
    memcpy(novaEntrada.enderecoIP, enderecoIP, 32);

    return novaEntrada;
}

void add(char* hostname, char *enderecoIP, TabelaDNS * DNS) {
    int posicaoHost = posicaoHostNaTabela(hostname, *DNS);

    if(posicaoHost > 0){ //host já existe na tabela, iremos atualizar os dados
        DNS->entradas[posicaoHost] = novaEntrada(hostname, enderecoIP);
        printf("As informações para o host informado foram atualizadas na tabela. \n");
        return;
    }

    DNS->entradas[DNS->nroEntradas] = novaEntrada(hostname, enderecoIP);
    DNS->nroEntradas += 1;
    
    printf("Host e endereço associado adicionados com sucesso.\n");

    // aumentar tamanho da tabela para próxima inserção
    void *tmp = realloc(DNS->entradas, (DNS->nroEntradas + 1) * sizeof(HostnameIP));
    if (tmp == NULL) {
        printf("Falha no realloc");
    } else {
        DNS->entradas = tmp;
    }


}

int posicaoHostNaTabela(char *hostname, TabelaDNS DNS){
    int comparacao; 

    for(int i = 0; i < DNS.nroEntradas; i++){
        comparacao = strcmp(hostname, DNS.entradas[i].hostname);
        if(comparacao == 0){
            return i;
        }
    }
    return -1;
}

void search(char *hostname, TabelaDNS DNS){
    int posicao = posicaoHostNaTabela(hostname, DNS);
    if(posicao > 0){
        printf("Endereço associado ao host %s: %s \n", DNS.entradas[posicao].hostname, DNS.entradas[posicao].enderecoIP);
        return ;
    }
    else{
        //TODO requisições para outros servidores
        printf("Endereço associado ao host %s não encontrado. \n", hostname);
    }

    return;

};

void link(char* ip, char *porta){
    printf("linking: %s at %s\n", ip, porta);
};


void resposta(int sockfd, TabelaDNS DNS){

    char buf[SIZE];
    memset(buf, 0, SIZE);

    int n = recv(sockfd, (char *)buf, SIZE, 0); 
    buf[n] = '\0'; 
    printf("Hostname to find : %s\n", buf);

    char *resposta = malloc(32);
    resposta[0] = '2';
    int posicao = posicaoHostNaTabela(buf, DNS);
    if(posicao > 0 ){
        strcat(resposta, DNS.entradas[posicao].enderecoIP);
    }
    else{
        strcat(resposta, "-1");
    }

    send(sockfd, resposta, strlen(resposta), 0);

    // n = recvfrom(sockfd, (char *)buf, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &client_addr, &client_addrlen); 
    
};

void requisicao(int sockfd, char * hostname){

    // int clientfd;
    // struct sockaddr_storage client_storage;
    // struct sockaddr *client_addr = (struct sockaddr *)(&client_storage);
    // socklen_t client_addrlen = sizeof(client_storage);
    char buf[SIZE];
    memset(buf, 0, SIZE);
    
    char *requisicao = malloc((strlen(hostname) + 1));
    requisicao[0] = '1';
    strcat(requisicao, hostname);
    send(sockfd, requisicao, strlen(requisicao), 0);
    // sendto(sockfd, (const char *)resposta, strlen(resposta),  MSG_CONFIRM, (const struct sockaddr *) &client_addr, client_addrlen);
    
    int n = recv(sockfd, (char *)buf, SIZE, 0);  
    printf("Hostname received : %s\n", buf);

}

int inicializarSocketAddr(const char *proto, const char *portstr, struct sockaddr_storage *storage) {
   
    uint16_t port = (uint16_t)atoi(portstr); 
    if (port == 0) {
        return -1;
    }
    port = htons(port); 

    memset(storage, 0, sizeof(*storage));
    if (0 == strcmp(proto, "v4")) {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_addr.s_addr = INADDR_ANY;
        addr4->sin_port = port;
        return 0;
    } else if (0 == strcmp(proto, "v6")) {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_addr = in6addr_any;
        addr6->sin6_port = port;
        return 0;
    } else {
        return -1;
    }
};

