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

struct HostnameIP novaEntrada(char* hostname, char *enderecoIP);

void add(char* hostname, char *enderecoIP, TabelaDNS * DNS);

int posicaoHostNaTabela(char *hostname, TabelaDNS DNS);

void search(char *hostname, TabelaDNS DNS);

void link(char* ip, char *porta);

void resposta(int sockfd, TabelaDNS DNS);

void requisicao(int sockfd, char * hostname);

int inicializarSocketAddr(const char *proto, const char *portstr, struct sockaddr_storage *storage);

#endif