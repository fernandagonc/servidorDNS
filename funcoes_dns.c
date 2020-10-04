#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "funcoes_dns.h"
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
        return DNS.entradas[posicao].enderecoIP;
    }

    return 0;
}

char * search(char *hostname, TabelaDNS DNS, TabelaLinks links){

    int hostEncontrado = 0;
    int localmente = 0;
    char * IP = searchLocal(hostname, DNS);

    if(IP != 0){
        hostEncontrado = 1;
        localmente = 1;

    }

    char buf[SIZE];
    char * requisicao;
    requisicao = malloc(SIZE);
    memset(requisicao, 0, SIZE);
    requisicao[0] = '1';
    strcat(requisicao, hostname);
    char *resposta;

    for(int i = 0; i < links.nroLinks; i++){
        if(hostEncontrado != 1){
            int socket = links.conexoes[i].sock;    
            struct sockaddr_storage address = links.conexoes[i].storage;
            socklen_t len = sizeof(&address);
            
            int send = sendto(socket, (const char *)requisicao, strlen(requisicao), MSG_CONFIRM, (const struct sockaddr *) &address, sizeof(address)); 
            if(send < 0){
                perror("Erro no send: ");
            }

            struct sockaddr_storage storage;
            memset(&storage, 0, sizeof(storage));  
            len = sizeof(&storage);

            memset(buf, 0, SIZE);
            recvfrom(socket, (char *)buf, SIZE, MSG_WAITALL,(struct sockaddr *) &storage, &len);

            if(charToInt(buf[0]) == 2){
                resposta = buf; 
                int i = 1;
                int length = strlen(buf);

                for( i = 1; i < length; i++){
                    resposta[i-1] = buf[i];
                }
                resposta[i-1] = '\0';

                if(resposta[0] == '-' && resposta[1] == '1'){
                    continue;

                }
                else{
                    hostEncontrado = 1;
                }
                
            }        

        }
        else{
            break;
        }
        
        
    }

    if (hostEncontrado == 1) {
        if(localmente == 1){
             return IP;
        } 
        return resposta;
    } else {
        return 0;
    }


};

struct ServerLinks novoLink(char* ip, char *porta){
    ServerLinks novoLink;

    int sock;
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        printf("\n Erro : Não foi possível criar o socket \n");
        exit(EXIT_FAILURE); 
    } 

    novoLink.sock = sock;
    memcpy(novoLink.porta, porta, 10);
    memcpy(novoLink.ip, ip, 33);

    if (0 != addrParse(novoLink.ip, atoi(novoLink.porta), &novoLink.storage)) {
        printf("Link inválido. \n");
        novoLink.sock = -1;
        return novoLink;
    }

    char str[SIZE];
    printAddr((const struct sockaddr *)&novoLink.storage, str, SIZE);
    printf("Linked to: %s\n", str);

    return novoLink;
}

void linkServers(char* ip, char *porta, TabelaLinks * links){
    ServerLinks link = novoLink(ip, porta);
    
    if (link.sock == -1){
        return;
    }

    links->conexoes[links->nroLinks] = link;
    links->nroLinks += 1;
    
    // aumentar tamanho da tabela para próxima inserção
    void *tmp = realloc(links->conexoes, (links->nroLinks + 1) * sizeof(ServerLinks));
    if (tmp == NULL) {
        printf("Falha no realloc");
    } else {
        links->conexoes = tmp;
    }
};