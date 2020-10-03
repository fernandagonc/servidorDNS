#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "funcoes_servidor.h"
#include "servidor.h"
#include "common.h"

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

void search(char *hostname, TabelaDNS DNS, TabelaLinks links, int * socket){

    char * IP = searchLocal(hostname, DNS);

    if(IP != 0){
        return;
    }
    
    else{

        char buf[SIZE];
        char * requisicao;
        requisicao = malloc(SIZE);
        memset(requisicao, 0, SIZE);
        requisicao[0] = '1';
        strcat(requisicao, hostname);

        for(int i = 0; i < links.nroLinks; i++){
        
            struct sockaddr_storage address;
            memset(&address, 0, sizeof(address)); 

            if (0 != addrParse(links.conexoes[i].ip, atoi(links.conexoes[i].porta), &address)) {
                exit(1);
            }
            

            char str[SIZE];
            socklen_t len = sizeof(&address);
            printAddr((const struct sockaddr *)&address, str, SIZE);
            printf("Send to Addr: %s \n", str);
            int send = sendto(*socket, (const char *)requisicao, strlen(requisicao), MSG_CONFIRM, (const struct sockaddr *) &address, sizeof(address)); 
            if(send < 0){
                perror("Erro no send: ");
            }
            else{
                printf("Enviada com sucesso!\n");
            }


            memset(buf, 0, SIZE);
            int recv =  recvfrom(*socket, (char *)buf, SIZE, MSG_WAITALL,(struct sockaddr *) &address, &len);

            if(recv > 0){
                printf("received: %s\n", buf);

                if(charToInt(buf[0]) == 2){
                    char *resposta = buf; 
                    int i = 1;
                    int length = strlen(buf);

                    for( i = 1; i < length; i++){
                        resposta[i-1] = buf[i];
                    }
                    resposta[i-1] = '\0';
                    printf("Resposta: %s \n", resposta);
                    if(resposta[0] == -1){
                        printf("Endereço associado ao host %s não encontrado no servidor de porta %s \n", hostname, links.conexoes[i].porta );

                    }
                    else{
                        printf("IP associado: %s\n", resposta);
                    }
            
                    
                }        

            }
        }
    }

    return;

};

struct ServerLinks novoLink(char* ip, char *porta){
    ServerLinks novoLink;
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