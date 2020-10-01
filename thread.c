#include "thread.h"
#include "funcoes_servidor.h"
#include "servidor.h"
#include "common.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#define PROTOCOLO "v4"
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
    

    ServerLinks serverSocket = criarSocket(port, ip);
    int sockfd = serverSocket.socket ;
    struct sockaddr_storage *client_storage = serverSocket.storage;
    socklen_t client_len = sizeof(*client_storage);
    
    struct sockaddr_in address; 
    memset(&address, 0, sizeof(address)); 

    struct in_addr inaddr4;
    inet_pton(AF_INET, ip, &inaddr4);

    address.sin_family = AF_INET; 
    address.sin_port = htons(atoi(port)); 
    address.sin_addr = inaddr4;

    printf("Porta da thread: %s ", args->porta);
    printf("Open to recv \n");
    while(1){
        int count = recvfrom(sockfd, (char *)buf, SIZE, MSG_WAITALL,(struct sockaddr *) &client_storage, &client_len);
        buf[count] = '\0'; 

        if(count > 0){
            printf("received: %s \n", buf);
            if(charToInt(buf[0]) == 1){
                char *host = buf; 
                int i = 1;
                int length = strlen(buf);

                for( i = 1; i < length; i++){
                    host[i-1] = buf[i];
                }
                host[i-1] = '\0';
                printf("search host: %s \n", host);
                char *IP;
                IP = malloc(33);
                memset(IP, 0, 33);

                char * resposta;
                resposta = malloc(100);
                memset(resposta, 0, 100);
                resposta[0] = '2';
             
                IP = searchLocal(host, args->DNS);
                printf("ip: %s \n", IP);
                if(IP == 0){
                    IP = "-1";
                }
                printf("IP: %s \n", IP);
                strcat(resposta, IP);


                int send = sendto(sockfd, resposta, sizeof(resposta), 0, (const struct sockaddr *)&client_storage, client_len);
                if(send < 0){
                    perror("Erro no send: ");
                }
                else{
                    printf("Resposta: %s enviada com sucesso!\n", resposta);
                    send = 0;
                    count = 0;
                    memset(buf, 0, SIZE);
                    //resposta = "";
                }
            }
       
            
            
        }
        

    }
   
    return 0;
}

// void * receberRequisicoes(void * argPointer){
//     char buf[SIZE];
//     ThreadArgs *args = argPointer;
//     int count = recv(args->socket, buf, SIZE, 0);
//     if(count > 0){
//         char *requisicao;
//         memmove(requisicao, buf+1, strlen (buf+1) + 1);
//         printf("buf: %s, requisicao: %s", buf, requisicao);
//         char *IP = (char *)searchLocal(requisicao, args->DNS);
//         char *resposta = "2";

//         if(IP != 0){
//             strcat(resposta, IP);
//         }
//         else{
//             strcat(resposta, "-1");
//         }
//         send(socket, resposta, sizeof(resposta), 0);
//     }

//     printf("entrei na requisicao");
//     free(args);
//     return;
// };
