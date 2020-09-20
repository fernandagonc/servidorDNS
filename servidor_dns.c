#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "funcoes_servidor.h"
#include "common.h"
#include <ctype.h> 

#define BUFSZ 1024
#define PROTOCOLO "v4"
#define MAXLINE 1024 
#define SIZE 1024
//https://www.geeksforgeeks.org/udp-server-client-implementation-c/
//https://sourcedaddy.com/networking/simple-dns-server.html


void usage() {
	printf("Uso:<porta> [startup]\n");
	printf("Exemplo de uso: 51511 inicializacao.txt\n");
	exit(EXIT_FAILURE);
}

void printTabelaDNS(TabelaDNS DNS){
    int i=0;
    for (i = 0; i < DNS.nroEntradas; i++){
        printf("Host: %s IP: %s \n", DNS.entradas[i].hostname, DNS.entradas[i].enderecoIP);
    };

};

int main(int argc, char *argv[]){

	if (argc < 2) {
        usage();
    }

    else if(argc == 3){
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
    add("host2", "192.255.168.1", &DNS);
    add("host3", "192.255.0.0", &DNS);

    printTabelaDNS(DNS);

    printf("\n");
    search("host2", DNS);
    search("host5", DNS);
    add("host3","155.155.0.0", &DNS);
    add("host4","111.111.111.111", &DNS);

    printf("\n");
    
    printTabelaDNS(DNS);


    char buf[BUFSZ];
    char *resposta = "hello";
    size_t count;
    int n, len;
    
    while (1){
           
        memset(buf, 0, BUFSZ);

        // int clientfd;
        // struct sockaddr_storage client_storage;
        // struct sockaddr *client_addr = (struct sockaddr *)(&client_storage);
        // socklen_t client_addrlen = sizeof(client_storage);

        // n = recvfrom(sockfd, (char *)buf, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &client_addr, &len); 
        // buf[n] = '\0'; 
        // printf("Client : %s\n", buf); 
        
        // sendto(sockfd, (const char *)resposta, strlen(resposta),  MSG_CONFIRM, (const struct sockaddr *) &client_addr, len);


    }
    
    close(sockfd);
    free(resposta);
    return 1;
};