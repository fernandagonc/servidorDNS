#include "thread.h"
#include "funcoes_dns.h"
#include "servidor.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SIZE 1024

void criarThread(void * argPointer) {
    pthread_t thread;
    ThreadArgs *args = argPointer;

    if (pthread_create(&thread, NULL, connectionHandler, (void *) args) < 0) {
        printf("Erro ao criar a thread.");
    }
    printf("Thread criada com sucesso! Inicializando socket para comunicação... \n");
}


void *connectionHandler(void *argPointer) {
    char buf[SIZE];
    ThreadArgs *args = argPointer;
    char* port = args->porta; 
    char* ip = args->ip;

    int sockfd = criarSocket(port, ip);
    args->socket = sockfd;

    char *host;  
    while(1){
        struct sockaddr_storage address; 
        memset(&address, 0, sizeof(address)); 
        socklen_t client_len = sizeof(address);

        int count = recvfrom(sockfd, (char *)buf, SIZE, MSG_WAITALL,(struct sockaddr *) &address, &client_len);
        buf[count] = '\0'; 
        
        if(count > 0){
            if(charToInt(buf[0]) == 1){

                host = buf; 
                int i = 1;
                int length = strlen(buf);

                for( i = 1; i < length; i++){
                    host[i-1] = buf[i];
                }
                host[i-1] = '\0';

                char *IP;
                IP = malloc(33);
                memset(IP, 0, 33);

                char * resposta;
                resposta = malloc(100);
                memset(resposta, 0, 100);
                resposta[0] = '2';
             
                IP = search(host, *args->DNS, *args->links);

                if(IP == 0){
                    char * notIP;
                    notIP = malloc(3);
                    memset(notIP, 0, 3);

                    notIP[0] = '-';
                    notIP[1] = '1';
                    notIP[2] = '\0';

                    strcat(resposta, notIP);


                }else{
                    strcat(resposta, IP);
                }
                
                int send = sendto(sockfd, (const char *)resposta, strlen(resposta), MSG_CONFIRM, (const struct sockaddr *) &address, sizeof(address)); 
                if(send < 0){
                    perror("Erro no send ");
                    exit(1);
                }
                else{
                    send = 0;
                    count = 0;
                    memset(buf, 0, SIZE);
                    resposta = "";
                }
                
            }
       
            
            
        }
        

    }
   
    return 0;
}