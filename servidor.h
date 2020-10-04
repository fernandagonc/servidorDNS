
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

void printAddr(const struct sockaddr *addr, char *str, size_t strsize);

int addrParse(const char *addrstr, int port, struct sockaddr_storage *storage);

int criarSocket(char * porta, char * ip);
 
int charToInt(char value);

#endif