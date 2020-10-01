#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "funcoes_servidor.h"
#include "servidor.h"

#define PROTOCOLO "v4"
#define SIZE 1024

void printTabelaDNS(TabelaDNS DNS){
    int i=0;
    for (i = 0; i < DNS.nroEntradas; i++){
        printf("Host: %s IP: %s \n", DNS.entradas[i].hostname, DNS.entradas[i].enderecoIP);
    };

};

void printTabelaLinks(TabelaLinks links){
    int i=0;
    for (i = 0; i < links.nroLinks; i++){
        printf("Porta: %s IP: %s \n", links.conexoes[i].porta, links.conexoes[i].ip);
    };

};

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

char *searchLocal(char *hostname, TabelaDNS DNS){
    int posicao = posicaoHostNaTabela(hostname, DNS);

    if(posicao >= 0){
        printf("Endereço associado ao host %s: %s \n", DNS.entradas[posicao].hostname, DNS.entradas[posicao].enderecoIP);
        return DNS.entradas[posicao].enderecoIP;
    }

    return 0;
}

void search(char *hostname, TabelaDNS DNS, TabelaLinks links){

    char * IP = searchLocal(hostname, DNS);

    if(IP != 0){
        return;
    }
    
    else{

        char buf[SIZE];

        for(int i = 0; i < links.nroLinks; i++){

            int sockfd = links.conexoes[i].socket ;
            struct sockaddr_storage *storage = links.conexoes[i].storage;
            socklen_t len = sizeof(&storage);

            printf("send p/ porta: %s\n", links.conexoes[i].porta);
            sendMessage(sockfd, links.conexoes[i].ip, atoi(links.conexoes[i].porta), hostname);
            memset(buf, 0, SIZE);
            int recv =  recvfrom(sockfd, (char *)buf, SIZE, MSG_WAITALL,(struct sockaddr *) &storage, &len);

            if(recv > 0){
                printf("Hostname received: %s\n", buf);
                break;
            }        
            
        }

        printf("Endereço associado ao host %s não encontrado. \n", hostname);
    }

    return;

};

// void requisicao(int sockfd, char * hostname){

//    // int clientfd;
//     char *ip = "192.168.0.0";
//    // char *porta = "52";
//     // struct sockaddr_storage server_storage;
//     // struct sockaddr *server_addr = (struct sockaddr *)(&server_storage);
//     // socklen_t server_addrlen = sizeof(server_storage);
//     struct sockaddr *server_addr = ip;
//     socklen_t server_addrlen = sizeof(ip);

//     char buf[SIZE];
//     memset(buf, 0, SIZE);
    
//     char *requisicao = malloc((strlen(hostname) + 1));
//     requisicao[0] = '1';
//     strcat(requisicao, hostname);
//     //send(sockfd, requisicao, strlen(requisicao), 0);
//     sendto(sockfd, requisicao, strlen(requisicao),  0, (const struct sockaddr *) &server_addr, server_addrlen);
    
//     if(recv(sockfd, (char *)buf, SIZE, 0)){
//         printf("Hostname received : %s\n", buf);
//     }

// }

struct ServerLinks novoLink(char* ip, char *porta){
    ServerLinks novoLink = criarSocket(porta, ip);
    memcpy(novoLink.porta, porta, 10);
    memcpy(novoLink.ip, ip, 33);

    return novoLink;
}

void linkServers(char* ip, char *porta, TabelaLinks * links){

    links->conexoes[links->nroLinks] = novoLink(ip, porta);
    links->nroLinks += 1;
    
    // aumentar tamanho da tabela para próxima inserção
    void *tmp = realloc(links->conexoes, (links->nroLinks + 1) * sizeof(ServerLinks));
    if (tmp == NULL) {
        printf("Falha no realloc");
    } else {
        links->conexoes = tmp;
    }
};


// void resposta(int sockfd, TabelaDNS DNS){

//     char buf[SIZE];
//     memset(buf, 0, SIZE);

//     int n = recv(sockfd, (char *)buf, SIZE, 0); 
//     buf[n] = '\0'; 
//     printf("Hostname to find : %s\n", buf);

//     char *resposta = malloc(32);
//     resposta[0] = '2';
//     int posicao = posicaoHostNaTabela(buf, DNS);
//     if(posicao > 0 ){
//         strcat(resposta, DNS.entradas[posicao].enderecoIP);
//     }
//     else{
//         strcat(resposta, "-1");
//     }

//     send(sockfd, resposta, strlen(resposta), 0);

//      //n = recvfrom(sockfd, (char *)buf, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &client_addr, &client_addrlen); 
    
// };



