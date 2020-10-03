#ifndef FUNCOES
#define FUNCOES

typedef struct HostnameIP{
    char hostname[50];
    char enderecoIP[32];
} HostnameIP;

typedef struct TabelaDNS {
    int nroEntradas;
    struct HostnameIP* entradas;
} TabelaDNS;

typedef struct ServerLinks {
    char porta[10];
    char ip[33];
    struct sockaddr_storage storage;
    int sock;
} ServerLinks;

typedef struct TabelaLinks {
    int nroLinks;
    struct ServerLinks * conexoes;
} TabelaLinks;

typedef struct ThreadArgs{
    char * porta;
    char * ip;
    TabelaDNS * DNS;
    TabelaLinks links;
    int socket;
} ThreadArgs;

void printTabelaLinks(TabelaLinks links);

void printTabelaDNS(TabelaDNS DNS);

struct HostnameIP novaEntrada(char* hostname, char *enderecoIP);

void add(char* hostname, char *enderecoIP, TabelaDNS * DNS);

int posicaoHostNaTabela(char *hostname, TabelaDNS DNS);

char * searchLocal(char *hostname, TabelaDNS DNS);

void search(char *hostname, TabelaDNS DNS, TabelaLinks links);

struct ServerLinks novoLink(char* ip, char *porta);

void linkServers(char* ip, char *porta, TabelaLinks *links);



#endif