all:
	gcc -Wall -c servidor.c
	gcc -Wall -c funcoes_dns.c
	gcc -Wall -c thread.c -lpthread
	gcc -Wall -Wextra -std=c11 -ggdb3 servidor_dns.c funcoes_dns.o servidor.o thread.o -o servidor_dns -lpthread

clean:
	rm servidor.o funcoes_dns.o thread.o servidor_dns
