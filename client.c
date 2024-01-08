#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#include "constants.h"
#include "operations_tcp_client.h"
#include "operations_udp_client.h"

#define PORT "58011"

User user;
Auction auction;

int send_udp;
long int bytes_img = 0;
char img_name[128] = {0};
int loop = 1;


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
            if ((buffer[1] == 'a') || strncmp("myauctions", buffer, 10) == 0)
                return CMD_MYAUCTIONS;
            else if ((buffer[1] == 'b') || strncmp("mybids", buffer, 6) == 0)
                return CMD_MYBIDS;
            else return CMD_ERROR;
        case 's':
            if ((buffer[1] == 'r' && buffer[2] == ' ') || strncmp("show_record", buffer, 11) == 0)
                return CMD_SHOW_RECORD;
            else if (buffer[1] == 'a' || strncmp("show_asset", buffer, 10) == 0)
                return CMD_SAS;
            else return CMD_ERROR;
        case 'o': // open
            return CMD_OPA;
        case 'c': //close
            return CMD_CLS;
        case 'b': //bid
            return CMD_BID;
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
            send_udp = 1;
            return client_login(buffer, &user);
        case CMD_LOGOUT:
            send_udp = 1;
            return client_logout(buffer, &user);
        case CMD_UNREGISTER:
            send_udp = 1;
            return client_unregister(buffer, &user);
        case CMD_MYAUCTIONS:
            send_udp = 1;
            return client_myauctions(buffer, &user);
        case CMD_MYBIDS:
            send_udp = 1;
            return client_mybids(buffer, &user);
        case CMD_SHOW_RECORD:
            send_udp = 1;
            return client_show_record(buffer);
        case CMD_LIST:
            send_udp = 1;
            memset(buffer, 0, 128);
            sprintf(buffer, "LST\n");
            return 0;
        case CMD_OPA:
            send_udp = 0;
            client_open(buffer, auction, user, img_name, &bytes_img);
            return 0;
        case CMD_CLS:
            send_udp = 0;
            bytes_img = 0;
            client_close(buffer, auction, user);
            return 0;
        case CMD_SAS:
            send_udp = 0;
            bytes_img = 0;
            client_show_asset(buffer, auction);
            return 0;
        case CMD_BID:
            send_udp = 0;
            bytes_img = 0;
            client_bid(buffer, user);
            return 0;
        case CMD_EXIT:
            if (user.logged_in) {
                printf("please logout first\n");
                return 1;
            }
            else loop = 0;
            return 0;
        default:
            printf("Invalid command: %s\n", buffer);
            return 1;
    }
}

enum Command get_answer_command(char *buffer){//clean up, user buffer[1]
    switch(buffer[1]){
        case 'L':
            if (buffer[2] == 'I')
                return CMD_LOGIN;
            else if (buffer[2] == 'O')
                return CMD_LOGOUT;
            else if (buffer[2] == 'S')
                return CMD_LIST;
            else return CMD_ERROR;
        case 'U':
            if (buffer[2] == 'R')
                return CMD_UNREGISTER;
            else return CMD_ERROR;
        case 'M':
            if (buffer[2] == 'A')
                return CMD_MYAUCTIONS;
            else if (buffer[2] == 'B')
                return CMD_MYBIDS;
            else return CMD_ERROR;
        case 'R':
            if (buffer[0] == 'E' && buffer[1] == 'R')
                return CMD_ERROR;
            else if (buffer[2] == 'C')
                return CMD_SHOW_RECORD;
            else return CMD_ERROR;
        case 'O':
            if (buffer[2] == 'A')
                return CMD_OPA;
            else return CMD_ERROR;
        case 'C':
            if (buffer[2] == 'L')
                return CMD_CLS;
            else return CMD_ERROR;
        case 'S':
            if (buffer[2] == 'A')
                return CMD_SAS;
            else return CMD_ERROR;
        case 'B':
            if (buffer[2] == 'D')
                return CMD_BID;
            else return CMD_ERROR;
        default:
            return CMD_ERROR;
    }
}

int execute_answer_client(char *buffer){
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
        case CMD_OPA:
            client_open_answer(buffer);
            break;
        case CMD_CLS:
            return client_close_answer(buffer);
        case CMD_SAS:
            client_show_asset_answer(buffer);
            break;
        case CMD_BID:
            client_bid_answer(buffer);
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
    int fd_udp, fd_tcp, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;

    struct sigaction act;

    memset(&act,0,sizeof act);
    act.sa_handler=SIG_IGN;

    if(sigaction(SIGPIPE,&act,NULL)==-1) exit(-1);

    char buffer[MAXLINE];

    fd_udp=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd_udp==-1) /*error*/exit(1);
    fd_tcp=socket(AF_INET,SOCK_STREAM,0); //TCP socket
    if(fd_tcp==-1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket
    
    errcode=getaddrinfo("localhost",PORT,&hints,&res);
    if(errcode!=0) /*error*/ exit(1);

    user.logged_in = 0;


    while(loop){
        memset(buffer, 0, sizeof(buffer));
        scanf(" %[^\n]", buffer);
        
        int do_continue = execute_commands_client(buffer);

        if (do_continue) continue;

        if (send_udp) { //case udp
            n=sendto(fd_udp, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
            if(n==-1) /*error*/ exit(1);

            memset(buffer, 0, MAXLINE);

            addrlen = sizeof(addr);
            n=recvfrom(fd_udp, buffer, MAXLINE, 0, (struct sockaddr*)&addr, &addrlen);
            if(n==-1) /*error*/ exit(1);
        }

        else { // case tcp
            n=connect(fd_tcp,res->ai_addr,res->ai_addrlen);
            if(n==-1)/*error*/ exit(1);
            
            n=write(fd_tcp,buffer,MAXLINE);
            if(n==-1) exit(1);
            memset(buffer, 0, MAXLINE);

            if (bytes_img > 0) {
                FILE *file = fopen(img_name, "rb");
                if (!file) {
                    perror("Error opening file");
                    continue;
                }
                size_t bytesRead;
                while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) { //send image
                    n=write(fd_tcp,buffer,MAXLINE);
                    memset(buffer, 0, MAXLINE);
                    if (bytesRead == bytes_img) break;
                }
                n=read(fd_tcp,buffer,MAXLINE);
                if(n==-1) exit(1);
                close(fd_tcp);
            }
            
        }

        execute_answer_client(buffer);

        
        //empty command and reply
        memset(buffer, 0, 128);

    }
    freeaddrinfo(res);
    close(fd_udp);
    return 0;
}