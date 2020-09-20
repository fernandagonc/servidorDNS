#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "common.h"
#include <ctype.h> 

#define BUFSZ 1024
#define PROTOCOLO "v4"
#define MAXLINE 1024 
#define SIZE 1024
//https://www.geeksforgeeks.org/udp-server-client-implementation-c/
//https://sourcedaddy.com/networking/simple-dns-server.html
int inicializarSocketAddr(const char *proto, const char *portstr, struct sockaddr_storage *storage);

void usage() {
	printf("Uso:<porta> [startup]\n");
	printf("Exemplo de uso: 51511 inicializacao.txt\n");
	exit(EXIT_FAILURE);
}

typedef struct HostnameIP{
    char hostname[50];
    char enderecoIP[32];
} HostnameIP;

typedef struct TabelaDNS {
    int nroEntradas;
    struct HostnameIP* entradas;
} TabelaDNS;

void printTabelaDNS(TabelaDNS DNS){
    int i=0;
    for (i = 0; i < DNS.nroEntradas; i++){
        printf("Host: %s IP: %s \n", DNS.entradas[i].hostname, DNS.entradas[i].hostname);
    };

};

struct HostnameIP novaEntrada(char* hostname, char *enderecoIP){
    HostnameIP novaEntrada;

    strncpy(novaEntrada.hostname, hostname, sizeof(hostname));
    strncpy(novaEntrada.enderecoIP, enderecoIP, sizeof(enderecoIP));

    return novaEntrada;
}

void *add(char* hostname, char *enderecoIP, TabelaDNS * DNS) {
    char* iterator;

    DNS->entradas[DNS->nroEntradas] = novaEntrada(hostname, enderecoIP);
    DNS->nroEntradas += 1;

    void *tmp = realloc(DNS->entradas, sizeof(DNS) + sizeof(DNS->entradas[0]));

    return;        
}

int main(int argc, char *argv[]){

	if (argc < 2) {
        usage();
    }
    else if(argv == 3){
        //abrir arquivo
        FILE *file = fopen(argv[2],"r");
        char linha[1024];  
        if( file == NULL ){                       
            return -1;
        }
        else{
            while(fgets(linha, 1024, file)) {

            }         

        }
    }

	struct sockaddr_storage storage;
    if (0 != inicializarSocketAddr(PROTOCOLO, argv[1], &storage)) {
        usage();
    }

	int sockfd;
    sockfd = socket(storage.ss_family, SOCK_STREAM, 0);
    
	if((sockfd) < 0){
        printf("\n Erro : Não foi possível criar o socket \n");
        exit(1);
    } 

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(sockfd, addr, sizeof(storage))) {
        printf("Erro no bind");
        exit(1);
    }

    TabelaDNS DNS;
    DNS.nroEntradas = 0;
    DNS.entradas = malloc(1 * sizeof(HostnameIP));

    add("host1", "192.240.168.0", &DNS);
    printTabelaDNS(DNS);

    char buf[BUFSZ];
    char *resposta = "hello";
    size_t count;
    int n, len;
    
    while (1){
           
        memset(buf, 0, BUFSZ);

        int clientfd;
        struct sockaddr_storage client_storage;
        struct sockaddr *client_addr = (struct sockaddr *)(&client_storage);
        socklen_t client_addrlen = sizeof(client_storage);

        n = recvfrom(sockfd, (char *)buf, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &client_addr, &len); 
        buf[n] = '\0'; 
        printf("Client : %s\n", buf); 
        
        sendto(sockfd, (const char *)resposta, strlen(resposta),  MSG_CONFIRM, (const struct sockaddr *) &client_addr, len);


    }
    
    close(sockfd);
    free(resposta);
    return 1;
};


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
