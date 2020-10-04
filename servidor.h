
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

int charToInt(char value);

void printAddr(const struct sockaddr *addr, char *str, size_t strsize);

int addrParse(const char *addrstr, int port, struct sockaddr_storage *storage);

int inicializarSocketAddr(const char *proto, const char *portstr, struct sockaddr_storage *storage, char * ip);

int criarSocket(char * porta, char * ip);
 

#endif