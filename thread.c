#include "thread.h"
#include "funcoes_servidor.h"
#include "servidor.h"
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

    int socket = criarSocket(port);
    printf("Porta da thread: %s ", args->porta);
    printf("Open to recv \n");

    int count = recv(socket, buf, SIZE, 0);
    if(count > 0){
        printf("received something ");
        char *requisicao = "";
        memmove(requisicao, buf+1, strlen (buf+1) + 1);
        printf("buf: %s, requisicao: %s", buf, requisicao);
        char *IP = (char *)searchLocal(requisicao, args->DNS);
        char *resposta = "2";

        if(IP != 0){
            strcat(resposta, IP);
        }
        else{
            strcat(resposta, "-1");
        }
        send(socket, resposta, sizeof(resposta), 0);
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
