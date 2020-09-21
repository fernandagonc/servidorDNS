#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "funcoes_servidor.h"
#include "common.h"
#include <ctype.h> 

#define PROTOCOLO "v4"
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

void chamarComando (char * comando, TabelaDNS *DNS){
    int j = 0, p = 0;
    char parametros[3][100]; 

    //separar parametros do comando 
    for(int i=0; i <= (strlen(comando));i++){
 
         if(comando[i]==' '||comando[i]=='\0'){
            parametros[p][j]='\0';
            p++;  //próximo parametro
            j = 0;    //inicio em zero para próximo parâmetro
        }
        else{
            parametros[p][j] = comando[i];
            j++;
        }
    }

    int comparacao = strcmp(parametros[0], "add");

    if(comparacao == 0){
        printf("add %s %s\n", parametros[1], parametros[2] );
        add(parametros[1], parametros[2], DNS);
        return;
    }
    
    comparacao = strcmp(parametros[0], "search");
    if(comparacao == 0){
        printf("search %s \n", parametros[1]);
        search(parametros[1], *DNS);
        return;
    }
    
    comparacao = strcmp(parametros[0], "link");
    if(comparacao == 0){
        printf("link \n");
        link(parametros[1], parametros[2]);
        return;
    }
    else{
        printf("Comando inválido \n");
        return;
    }
};


int main(int argc, char *argv[]){

	if (argc < 2) {
        usage();
    }
    
    TabelaDNS DNS;
    DNS.nroEntradas = 0;
    DNS.entradas = malloc(1 * sizeof(HostnameIP));
   
    struct sockaddr_storage storage;
    if (0 != inicializarSocketAddr(PROTOCOLO, argv[1], &storage)) {
        usage();
    }

	int sockfd;
    sockfd = socket(storage.ss_family, SOCK_DGRAM, 0);
    
	if((sockfd) < 0){
        printf("\n Erro : Não foi possível criar o socket \n");
        exit(1);
    } 

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(sockfd, addr, sizeof(storage))) {
        printf("Erro no bind");
        exit(1);
    }
        
    if(argc == 3){
        //abrir arquivo
        FILE *file = fopen(argv[2],"r");
        char linha[1024];  
        if( file == NULL ){                       
            return -1;
        }
        else{
            while(fgets(linha, 1024, file)) {
                linha[strcspn(linha, "\n")] = 0;
                chamarComando(linha, &DNS);
                printTabelaDNS(DNS);
                printf("\n");
            }       

        }
    }

    while(1){
        printf("\nComando> ");
	    char comando[50];
	    if(fgets(comando, 50, stdin)){
            comando[strcspn(comando, "\n")] = 0;
            chamarComando(comando, &DNS);
        }
    }
    
    
    return 1;
};