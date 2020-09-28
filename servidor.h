
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

int inicializarSocketAddr(const char *proto, const char *portstr, struct sockaddr_storage *storage);

int criarSocket(char * porta);


#endif