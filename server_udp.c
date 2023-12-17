#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#define PORT "58011"


#include "users.h"
#include "auction.h"

//new
#include <fcntl.h>
#include <stdio.h>

#include "constants.h"
#include "operations_udp_server.h"

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
        case CMD_EXIT:
            return 1;
        default:
            sprintf(buffer, "ERR\n");
    }
    return 0;
}

int main(){
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[128];

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_flags = AI_PASSIVE;

    errcode=getaddrinfo(NULL, PORT, &hints, &res);
    if(errcode!=0) /*error*/ exit(1);
    
    n=bind(fd,res->ai_addr, res->ai_addrlen);
    if(n==-1) /*error*/ exit(1);

    initUsers();
    initAuctions();
    
    while (1){
        addrlen = sizeof(addr);
        memset(buffer, 0, 128);
        n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if(n==-1)/*error*/exit(1);

        write(1, "received: ", 10); write(1, buffer, n);

        if (executeCommands(buffer)) break;

        printf("sending: %s", buffer);
        
        n = sendto(fd, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, addrlen);
        if(n==-1)/*error*/exit(1);


    }

    freeaddrinfo(res);
    close(fd);
    
    closeUsers();
    return 0;
}