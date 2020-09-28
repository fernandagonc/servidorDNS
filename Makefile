all:
	gcc -Wall -c common.c
	gcc -Wall -c servidor.c
	gcc -Wall -c funcoes_servidor.c
	gcc -Wall -c thread.c -lpthread
	gcc -Wall -Wextra -std=c11 -ggdb3 servidor_dns.c common.o funcoes_servidor.o servidor.o	thread.o -o servidor_dns -lpthread

run: all

clean:
	rm common.o servidor.o funcoes_servidor.o thread.o servidor_dns
