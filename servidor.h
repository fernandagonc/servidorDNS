
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#ifndef SERVIDOR
#define SERVIDOR

int inicializarSocketAddr(const char *proto, const char *portstr, struct sockaddr_storage *storage, char * ip);

ServerLinks criarSocket(char * porta, char * ip);

void sendMessage(int socket, char * ip, int port, char* message);

void receiveMessage(int socket, struct sockaddr *address);


#endif