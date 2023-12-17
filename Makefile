CC = gcc

# Para mais informações sobre as flags de warning, consulte a informação adicional no lab_ferramentas
CFLAGS = -g -std=c17 -D_POSIX_C_SOURCE=200809L \
		 -Wall -Werror -Wextra 


all: client server_udp server_tcp

client: client.c 
	$(CC) $(CFLAGS) $(SLEEP) -o client client.c operations_udp_client.c operations_tcp_client.c

server_udp: server_udp.c users.c
	$(CC) $(CFLAGS) $(SLEEP) -o server_udp server_udp.c users.c operations_udp_server.c auction.c

server_tcp: server_tcp.c users.c
	$(CC) $(CFLAGS) $(SLEEP) -o server_tcp server_tcp.c users.c operations_tcp_server.c auction.c

%.o: %.c %.h
	$(CC) $(CFLAGS) -c ${@:.o=.c}


clean:
	rm -f *.o client server_udp

format:
	@which clang-format >/dev/null 2>&1 || echo "Please install clang-format to run this command"
	clang-format -i *.c *.h

