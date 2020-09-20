all:
	gcc -Wall -c common.c
	gcc -Wall -Wextra -std=c11 -ggdb3 cliente.c common.o -o cliente
	gcc -Wall -Wextra -std=c11 -ggdb3 servidor_dns.c common.o -o servidor_dns

run: all