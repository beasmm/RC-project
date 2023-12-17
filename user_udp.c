#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//#define PORT "58011"

#define PORT "58011"


#include "constants_udp.h"
#include "operations_udp_client.h"

//global variables
User user;


enum Command get_client_command(char *buffer){
    switch(buffer[0]){
        case 'l':
            if (strncmp("login", buffer, 5) == 0)
                return CMD_LOGIN;
            else if (strcmp("logout", buffer) == 0)
                return CMD_LOGOUT;
            else if (strcmp("list", buffer) == 0 || buffer[1] == '\0')
                return CMD_LIST;
            else return CMD_ERROR;
        case 'u':
            if (strcmp("unregister", buffer) == 0)
                return CMD_UNREGISTER;
            else return CMD_ERROR;
        case 'm':
            if ((buffer[1] == 'a' && buffer[2] == ' ') || strncmp("myauctions", buffer, 10) == 0)
                return CMD_MYAUCTIONS;
            else if ((buffer[1] == 'b' && buffer[2] == ' ') || strncmp("mybids", buffer, 6) == 0)
                return CMD_MYBIDS;
            else return CMD_ERROR;
        case 's':
            if ((buffer[1] == 'r' && buffer[2] == ' ') || strncmp("show_record", buffer, 11) == 0)
                return CMD_SHOW_RECORD;
            else return CMD_ERROR;
        case 'e':
            if(strcmp("exit", buffer) == 0)
                return CMD_EXIT;
            return CMD_ERROR;
        default:
            return CMD_ERROR;
    }
}

int execute_commands_client(char *buffer){
    switch(get_client_command(buffer)){
        case CMD_LOGIN:
            return client_login(buffer, &user);
        case CMD_LOGOUT:
            return client_logout(buffer, &user);
        case CMD_UNREGISTER:
            return client_unregister(buffer, &user);
        case CMD_MYAUCTIONS:
            return client_myauctions(buffer);
        case CMD_MYBIDS:
            return client_mybids(buffer);
        case CMD_SHOW_RECORD:
            return client_show_record(buffer);
        case CMD_LIST:
            memset(buffer, 0, 128);
            sprintf(buffer, "LST\n");
            return 0;
        case CMD_EXIT:
            if (user.logged_in) {
                printf("please logout first\n");
                return 1;
            }
            else exit(0);
            return 0;
        default:
            printf("Invalid command: %s\n", buffer);
            return 1;
    }
}
//TODO: change password size

enum Command get_answer_command(char *buffer){
    switch(buffer[0]){
        case 'R':
            if (buffer[1] == 'L' && buffer[2] == 'I')
                return CMD_LOGIN;
            else if (buffer[1] == 'L' && buffer[2] == 'O')
                return CMD_LOGOUT;
            else if (buffer[1] == 'U' && buffer[2] == 'R')
                return CMD_UNREGISTER;
            else if (buffer[1] == 'M' && buffer[2] == 'A')
                return CMD_MYAUCTIONS;
            else if (buffer[1] == 'M' && buffer[2] == 'B')
                return CMD_MYBIDS;
            else if (buffer[1] == 'S' && buffer[2] == 'R')
                return CMD_SHOW_RECORD;
            else if (buffer[1] == 'L' && buffer[2] == 'S')
                return CMD_LIST;
            else return CMD_ERROR;
        case 'E':
            if (buffer[1] == 'R' && buffer[2] == 'R')
                return CMD_ERROR;
            else return CMD_ERROR;
        default:
            return CMD_ERROR;
    }
}

int get_answer(char *buffer){
    switch(get_answer_command(buffer)){
        case CMD_LOGIN:
            if(client_login_answer(buffer)) user.logged_in = 1;
            break;
        case CMD_LOGOUT:
            if(client_logout_answer(buffer)) user.logged_in = 0;
            break;
        case CMD_UNREGISTER:
            if(client_unregister_answer(buffer)) user.logged_in = 0;
            break;
        case CMD_MYAUCTIONS:
            client_myauctions_answer(buffer);
            break;
        case CMD_MYBIDS:
            client_mybids_answer(buffer);
            break;
        case CMD_SHOW_RECORD:
            client_show_record_answer(buffer);
            break;
        case CMD_LIST:
            client_list_answer(buffer);
            break;
        case CMD_EXIT:
            exit(0);
            break;
        default:
            break;
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
    char command[30];

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket
    
    errcode=getaddrinfo("localhost",PORT,&hints,&res);
    if(errcode!=0) /*error*/ exit(1);

    user.logged_in = 0;

    //scanf("%s %s %s", cmd, uid, password);

    while(1){

        memset(command, 0, sizeof(command));
        scanf(" %[^\n]", command);

        int do_continue = execute_commands_client(command);

        if (do_continue) continue;

        printf("command: %s", command);
        
        n=sendto(fd, command, strlen(command), 0, res->ai_addr, res->ai_addrlen);
        if(n==-1) /*error*/ exit(1);

        addrlen = sizeof(addr);
        n=recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if(n==-1) /*error*/ exit(1);

        printf("received: %s", buffer);

        get_answer(buffer);

        
        //empty command and reply
        memset(buffer, 0, 128);

    }
    freeaddrinfo(res);
    close(fd);
    return 0;
}
