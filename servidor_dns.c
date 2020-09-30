#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "funcoes_servidor.h"
#include "common.h"
#include "servidor.h"
#include "thread.h"
#include <unistd.h>

#define PROTOCOLO "v4"
#define SIZE 1024

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

void printTabelaLinks(TabelaLinks links){
    int i=0;
    for (i = 0; i < links.nroLinks; i++){
        printf("Porta: %s \n", links.conexoes[i].porta);
    };

};


void chamarComando (char * comando, TabelaDNS *DNS, TabelaLinks *links){
    int j = 0, p = 0;
    char parametros[3][100]; 
    int length = strlen(comando);
    //separar parametros do comando 
    for(int i = 0; i <= length; i++){
 
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
        printf("add %s %s\n", parametros[1], parametros[2]);
        add(parametros[1], parametros[2], DNS);
        return;
    }
    
    comparacao = strcmp(parametros[0], "search");
    if(comparacao == 0){
        printf("search %s \n", parametros[1]);
        search(parametros[1], *DNS, *links);

        return;
    }
    
    comparacao = strcmp(parametros[0], "link");
    if(comparacao == 0){
        printf("link \n");
        linkServers(parametros[1], parametros[2], links);
        printTabelaLinks(*links);
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
   
    TabelaLinks links;
    links.nroLinks = 0;
    links.conexoes = malloc(1 * sizeof(ServerLinks));

    ThreadArgs args;
    ThreadArgs *args_addr;

    args.porta = argv[1];
    args.DNS = DNS;
    args_addr = &args;
    criarThread(args_addr);
    sleep(1);

    char linha[1024];  

    if(argc == 3){
        //abrir arquivo
        FILE *file = fopen(argv[2],"r");
        if( file == NULL ){                       
            return -1;
        }
        else{
            while(fgets(linha, 1024, file)) {
                linha[strcspn(linha, "\n")] = 0;
                chamarComando(linha, &DNS, &links);
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
            chamarComando(comando, &DNS, &links);
            printTabelaDNS(DNS);
            printf("\n");

        }      
        
    }
    
    
    return 1;
};