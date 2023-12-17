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
#define PORT 58011


#include "users.h"
#include "auction.h"
#include "constants.h"
#include "operations_tcp_server.h"
#include "operations_udp_server.h"

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
                printf("logout\n");
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
            if( (strcmp(buffer, "exit\n") == 0) || (strcmp(buffer, "exit") == 0) )
                return CMD_EXIT;
            else
                return CMD_ERROR;
        default:
            return CMD_ERROR;
    }
}

int executeCommands(char *buffer){
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
            open_server(buffer);
            break;
        case CMD_CLS:
            close_server(buffer);
            break;
        case CMD_SAS:
            show_asset_server(buffer);
            break;
        case CMD_BID:
            bid_server(buffer);
            break;
        case CMD_EXIT:
            return 1;
        default:
            sprintf(buffer, "ERR\n");
    }
    return 0;
}


int main() { 
	int listenfd, newfd, udpfd, nready, maxfdp1; 
	char buffer[MAXLINE]; 
	pid_t childpid; 
	fd_set rset; 
	ssize_t n; 
	socklen_t len; 
	const int on = 1; 
	struct sockaddr_in cliaddr, servaddr; 
	void sig_chld(int); 
    
    char buffer_to_send[MAXLINE]={0};
    char state[9][3]={"NOK","NLG","OK","EAU","END","ACC","REF","ILG","ERR"};
    int counter_auctions=1;

	/* create listening TCP socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0); 
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// binding server addr structure to listenfd 
	bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 
	listen(listenfd, 10); 

	/* create UDP socket */
	udpfd = socket(AF_INET, SOCK_DGRAM, 0); 
	// binding server addr structure to udp sockfd 
	bind(udpfd, (struct sockaddr*)&servaddr, sizeof(servaddr)); 

	// clear the descriptor set 
	FD_ZERO(&rset); 

	// get maxfd 
	maxfdp1 = max(listenfd, udpfd) + 1; 

    initUsers();

	for (;;) { 

		// set listenfd and udpfd in readset 
		FD_SET(listenfd, &rset); 
		FD_SET(udpfd, &rset); 

		// select the ready descriptor 
		nready = select(maxfdp1, &rset, NULL, NULL, NULL); 

		// if tcp socket is readable then handle 
		// it by accepting the connection 
		if (FD_ISSET(listenfd, &rset)) { 
			len = sizeof(cliaddr); 
			if ((newfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1) exit(1); 
			if ((childpid = fork()) == 0) { 
                char code[3]={0};
                char status[3]={0};     
				close(listenfd); 
				bzero(buffer, sizeof(buffer)); 
				n = read(newfd, buffer, sizeof(buffer)); 
                if (n==-1) exit(1);
				printf("Receiving: %s\n",buffer);
                executeCommands(buffer);
                n=write(newfd, buffer, MAXLINE);
                if(n==-1) exit(1);
			} 
			close(newfd); 
		} 
		// if udp socket is readable receive the message. 
		if (FD_ISSET(udpfd, &rset)) { 
			len = sizeof(cliaddr); 
			bzero(buffer, sizeof(buffer));  
			n = recvfrom(udpfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len); 
            if (n==-1) exit(1);
            write(1, "received: ", 10); write(1, buffer, n);
            executeCommands(buffer);
            printf("sending: %s", buffer);
			n = sendto(udpfd, (const char*)buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr)); 
            if(n==-1)/*error*/exit(1);

		} 
	} 
    close(udpfd);
    close(listenfd);
    
    closeUsers();
    return 0;
} 
