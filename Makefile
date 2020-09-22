all:
	gcc -Wall -c common.c
	gcc -Wall -c funcoes_servidor.c
	gcc -Wall -Wextra -std=c11 -ggdb3 servidor_dns.c common.o funcoes_servidor.o -o servidor_dns

run: all

clean:
	rm common.o funcoes_servidor.o servidor_dns
