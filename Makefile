CC = gcc

# Para mais informações sobre as flags de warning, consulte a informação adicional no lab_ferramentas
CFLAGS = -Wall -Werror


all: user AS

user: client.c 
	$(CC) $(CFLAGS) $(SLEEP) -o user client.c operations_udp_client.c operations_tcp_client.c

AS: AS.c
	$(CC) $(CFLAGS) $(SLEEP) -o AS AS.c users.c operations_udp_server.c auction.c operations_tcp_server.c


%.o: %.c %.h
	$(CC) $(CFLAGS) -c ${@:.o=.c}


clean:
	rm -f *.o user AS

delete:
	rm -r USERS AUCTIONS

format:
	@which clang-format >/dev/null 2>&1 || echo "Please install clang-format to run this command"
	clang-format -i *.c *.h

