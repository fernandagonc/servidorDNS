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
    int socket;
    struct sockaddr_storage *storage;
} ServerLinks;

typedef struct TabelaLinks {
    int nroLinks;
    struct ServerLinks * conexoes;
} TabelaLinks;

typedef struct ThreadArgs{
    char * porta;
    TabelaDNS DNS;
    TabelaLinks links;

} ThreadArgs;

struct HostnameIP novaEntrada(char* hostname, char *enderecoIP);

void add(char* hostname, char *enderecoIP, TabelaDNS * DNS);

int posicaoHostNaTabela(char *hostname, TabelaDNS DNS);

char * searchLocal(char *hostname, TabelaDNS DNS);

void search(char *hostname, TabelaDNS DNS, TabelaLinks links);

struct ServerLinks novoLink(char* ip, char *porta);

void linkServers(char* ip, char *porta, TabelaLinks *links);

void resposta(int sockfd, TabelaDNS DNS);

void requisicao(int sockfd, char * hostname);


#endif