CC = gcc

# Para mais informações sobre as flags de warning, consulte a informação adicional no lab_ferramentas
CFLAGS = -g -std=c17 -D_POSIX_C_SOURCE=200809L \
		 -Wall -Werror -Wextra 


all: user server

user: user_udp.c 
	$(CC) $(CFLAGS) $(SLEEP) -o udp user_udp.c

server: server_udp.c users.c
	$(CC) $(CFLAGS) $(SLEEP) -o server server_udp.c users.c

%.o: %.c %.h
	$(CC) $(CFLAGS) -c ${@:.o=.c}


clean:
	rm -f *.o udp server

format:
	@which clang-format >/dev/null 2>&1 || echo "Please install clang-format to run this command"
	clang-format -i *.c *.h

