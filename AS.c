// Server program 
#include <arpa/inet.h> 
#include <errno.h> 
#include <netinet/in.h> 
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>

#define DEFAULT_PORT_G7 58007


#include "users.h"
#include "auction.h"
#include "operations_server.h"

int read_file = 0, send_file = 0;
char file_name[25] = {0};

int max(int x, int y) 
{ 
	if (x > y) 
		return x; 
	else
		return y; 
} 

enum Command get_command(char *buffer){
    switch(buffer[0]){
        case 'L':
            if (buffer[1] == 'I' && buffer[2] == 'N')
                return CMD_LOGIN;
            else if (buffer[1] == 'O' && buffer[2] == 'U'){
                return CMD_LOGOUT;
            }
            else if (buffer[1] == 'S' && buffer[2] == 'T')
                return CMD_LIST;
            else if (buffer[1] == 'M' && buffer[2] == 'A')
                return CMD_MYAUCTIONS;
            else if (buffer[1] == 'M' && buffer[2] == 'B')
                return CMD_MYBIDS;
            else
                return CMD_ERROR;
        case 'U':
            if (buffer[1] == 'N' && buffer[2] == 'R')
                return CMD_UNREGISTER;
            else
                return CMD_ERROR;
        case 'S':
            if (buffer[1] == 'R' && buffer[2] == 'C')
                return CMD_SHOW_RECORD;
            else if (buffer[1] == 'A' && buffer[2] == 'S')
                return CMD_SAS;
            else
                return CMD_ERROR;
        case 'O':
            if (buffer[1] == 'P' && buffer[2] == 'A')
                return CMD_OPA;
            else
                return CMD_ERROR;
        case 'C':
            if (buffer[1] == 'L' && buffer[2] == 'S')
                return CMD_CLS;
            else
                return CMD_ERROR;
        case 'B':
            if (buffer[1] == 'I' && buffer[2] == 'D')
                return CMD_BID;
            else
                return CMD_ERROR;
        case 'e':
            if((strcmp(buffer, "exit\n") == 0) || (strcmp(buffer, "exit") == 0))
                return CMD_EXIT;
            else
                return CMD_ERROR;
        default:
            return CMD_ERROR;
    }
}

int executeCommands(char *buffer){
    int ret = 0;
    switch(get_command(buffer)){
        case CMD_LOGIN:
            login(buffer);
            break;
        case CMD_LOGOUT:
            logout(buffer);
            break;
        case CMD_UNREGISTER:
            unregister(buffer);
            break;
        case CMD_MYAUCTIONS:
            myauctions(buffer);
            break;
        case CMD_MYBIDS:
            mybids(buffer);
            break;
        case CMD_SHOW_RECORD:
            show_record(buffer);
            break;
        case CMD_LIST:
            list(buffer);
            break;
        case CMD_OPA:
            ret = open_server(buffer, getAuctionID(), file_name);
            read_file = 1;
            break;
        case CMD_CLS:
            close_server(buffer);
            break;
        case CMD_SAS:
            ret = show_asset_server(buffer, file_name);
            send_file = 1;
            break;
        case CMD_BID:
            bid_server(buffer);
            break;
        case CMD_EXIT:
            return 1;
        default:
            sprintf(buffer, "ERR\n");
    }
    return ret;
}


int main(int argc, char **argv) { 
    int port = 0;
    int verbose = 0;

    int opt;
    while ((opt = getopt(argc, argv, "p:v")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                break;
        }
    }

    if (port == 0) port = DEFAULT_PORT_G7;

    printf("port: %d\n", port);
    printf("verbose: %d\n", verbose);

    initUsers();
    initAuctions();

	int listenfd, newfd, udpfd, maxfdp1; 
	char buffer[MAXLINE]; 
	fd_set rset; 
	ssize_t n; 
	socklen_t len; 
	struct sockaddr_in cliaddr, servaddr; 

	/* create listening TCP socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0); 
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET; 
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(port); 

    // Set SO_REUSEADDR option to allow quick restart
    int enabled = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) < 0) {
        close(listenfd);
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(1);
    }
	// binding server addr structure to listenfd 
	n = bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
    if(n == -1){
        perror("Failed to bind listenfd");
        exit(1);
    }
    n = listen(listenfd, 10);
    if(n == -1){
        perror("Failed to listen");
        exit(1);
    }

	/* create UDP socket */
	udpfd = socket(AF_INET, SOCK_DGRAM, 0); 

	// binding server addr structure to udp sockfd 
	n = bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
    if(n == -1){
        perror("Failed to bind udpfd");
        exit(1);
    }

	while (1) {
        // clear the descriptor set 
	    FD_ZERO(&rset); 
        
		// set listenfd and udpfd in readset 
		FD_SET(listenfd, &rset); 
		FD_SET(udpfd, &rset); 

        // get maxfd 
        maxfdp1 = max(listenfd, udpfd) + 1; 

		// select the ready descriptor 
		int ready = select(maxfdp1, &rset, NULL, NULL, NULL); 
        if (ready < 0){
            perror("select");
            return 1;
        }

		// if tcp socket is readable then handle 
		// it by accepting the connection 
		if (FD_ISSET(listenfd, &rset)) { 
            printf("TCP\n");
			len = sizeof(cliaddr); 
			if ((newfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1) {
                perror("Failed to accept");
                exit(1); 
            }

            memset(buffer, 0, sizeof(buffer));
            n = read(newfd, buffer, sizeof(buffer)); 
            if (n == -1) {
                perror("Failed to read");
                exit(1);
            }
            if (verbose){
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(cliaddr.sin_addr), clientIP, INET_ADDRSTRLEN);
                printf("Accepted TCP connection from IP: %s, Port: %d\n", clientIP, ntohs(cliaddr.sin_port));
            }
            write(1, "Received: ", 10); write(1, buffer, n);
            int f_size = executeCommands(buffer);
            n = write(newfd, buffer, MAXLINE);
            if (read_file && strcmp(buffer, "ROA NOK") != 0 && strcmp(buffer, "ROA NLG") != 0) {
                server_receiveFile(newfd, file_name, f_size);
            }
            read_file = 0;
            if (send_file && strncmp(buffer, "RSA OK ", 7) == 0) {
                server_sendFile(newfd, file_name, f_size);
            }
            send_file = 0;
            n = write(newfd, buffer, MAXLINE);
            if(n == -1) {
                perror("Failed to write");
                exit(1);
            }
			close(newfd); 
            newfd = -1;
		} 
		// if udp socket is readable receive the message. 
		if (FD_ISSET(udpfd, &rset)) { 
            printf("UDP\n");
			len = sizeof(cliaddr); 
            memset(buffer, 0, sizeof(buffer)); 
			n = recvfrom(udpfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len); 
            if (n == -1) {
                perror("Failed to read");
                exit(1);
            }
            if (verbose){
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(cliaddr.sin_addr), clientIP, INET_ADDRSTRLEN);
                printf("Received UDP message from IP: %s, Port: %d\n", clientIP, ntohs(cliaddr.sin_port));
            }
            write(1, "Received: ", 10); write(1, buffer, n);
            executeCommands(buffer);
            printf("Sending: %s", buffer);
			n = sendto(udpfd, (const char*)buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
            if(n == -1){
                perror("Failed to send");
                exit(1);
            }
		} 
	} 
    close(udpfd);
    close(listenfd);
    
    closeUsers();
    return 0;
} 
