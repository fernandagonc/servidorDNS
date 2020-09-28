
#include <pthread.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "funcoes_servidor.h"
#include "servidor.h"
#ifndef THREAD
#define THREAD

void criarThread(void* argPointer);

void *connectionHandler(void *);


#endif