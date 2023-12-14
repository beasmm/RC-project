#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define PORT "58011"


#include "constants_udp.h"

//global variables
User user;


int get_word(char str[], int is_id){
    char c = getchar();
	int i = 0;
	while (c == ' ')
		c = getchar();
	while (c != ' ' && c != '\n') {
        if (is_id && (c < '0' || c > '9')) {
            printf("invalid uid\n");
            return -1;
        }
		str[i++] = c;
		c = getchar();
	}
	str[i] = '\0';
    if (is_id && strlen(str) != UID_SIZE) {
        printf("invalid uid size\n");
        return -1;
    }
    if (!is_id && strlen(str) != PASSWORD_SIZE) {
        printf("iinvalid password size\n");
        return -1;
    }
	return 0;
}



void get_message(int case_id, char str[]){
    char cmd[8][CODE_SIZE] = {"LIN", "LOU", "UNR", "LMA", "LMB", "SRC"};

    strcpy(str, cmd[case_id]);
    strcat(str, " ");
    strcat(str, user.uid);
    
    //specific to cases login, logout and unregister
    if (case_id < 3) {
        strcat(str, " ");
        strcat(str, user.password);
    }

    strcat(str, "\n");
    return;
}



void print_auctions(char str[]){
    char aid[4];
    int i = 7, j = 0;

    //while(str[i] != '\n' && str[i] != '\0') {
    while(str[i] != '\0' && str[i] != '\n') {
        while (j < 3) {
            aid[j++] = str[i++];
            if (str[i] < '0' || str[i] > '9') break;
        }

        if (str[i] != ' ' && (str[i] < '0' || str[i] > '9')) break;

        aid[j] = '\0';
        i++;

        printf("id: %s --> ", aid);

        if (str[i] == '0') printf("state: inactive\n");
        else if (str[i] == '1') printf("state: active\n");
        j = 0;
        i += 2;
    }
    printf("\n");

    return;
}


int get_output(int case_id, char cmd[], char reply[]){
    if (strcmp("ERR", reply) == 0) {
        printf("server error\n");
        return 0;
    }
    //case login: change logged_in to 1
    if (case_id == CASE_LOGIN) {
        if (strcmp("OK", reply) == 0) {
            printf("successful %s\n", cmd);
            user.logged_in = 1;
            return 0;
        }
        else if (strcmp("NOK", reply) == 0) {
            printf("incorrect login attempt\n");
            return 0;
        }
        else {
            printf("new user registered\n");
            user.logged_in = 1;
            return 0;
        }
    }
    //cases logout and unregister
    else if (case_id == CASE_LOGOUT || case_id == CASE_UNREGISTER) {
        if (strcmp("OK", reply) == 0) {
            printf("successful %s\n", cmd);
            if (case_id == CASE_LOGOUT) user.logged_in = 0; //logout
            return 0;
        }
        else if (strcmp("UNR", reply) == 0) {
            printf("unknow user\n");
            return 0;
        }
        //logout 
        else if (case_id == CASE_LOGOUT && strcmp("NOK", reply) == 0) {
            printf("user not logged in\n");
            return 0;
        }
        //unregister 
        else {
            printf("or incorrect unregister attempt\n");
            return 0;
        }
    }
    //cases myauctions and mybids
    else if (case_id == CASE_MYAUCTIONS || case_id == CASE_MYBIDS) {
        if (strcmp("NLG", reply) == 0) {
            printf("user not logged in\n");
            return 0;
        }
        else if (strcmp("NOK", reply) == 0) {
            printf("user has no ongoing bids\n");
            return 0;
        }
        else return 1;
    }

    //case list
    else if (case_id == CASE_LIST) {
        if (strcmp("NOK", reply) == 0) {
            printf("no auction started yet\n");
            return 0;
        }
        else {
            printf("auctions:\n");
            return 2;
        }
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
    //char output[128];

    int case_id;
    char cmd[MAX_CMD_SIZE], reply[128];
    char command[CODE_SIZE + UID_SIZE + PASSWORD_SIZE];

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket
    
    errcode=getaddrinfo("tejo.tecnico.ulisboa.pt",PORT,&hints,&res);
    if(errcode!=0) /*error*/ exit(1);

    //scanf("%s %s %s", cmd, uid, password);

    while(1){
        get_word(cmd);

        if(strcmp("login", cmd) == 0){
            case_id = CASE_LOGIN;
            if (get_word(user.uid, 1) == -1) 
                continue;
            if (get_word(user.password, 0) == -1) 
                continue;
            get_message(case_id, command);
        }
        
        else if(strcmp("logout", cmd) == 0){
            case_id = CASE_LOGOUT;
            get_message(case_id, command);
        }

        else if(strcmp("unregister", cmd) == 0){
            case_id = CASE_UNREGISTER;
            get_message(case_id, command);
        }

        else if(strcmp("myauctions", cmd) == 0 || strcmp("ma", cmd) == 0){
            case_id = CASE_MYAUCTIONS;
            get_message(case_id, command);
        }

        else if(strcmp("mybids", cmd) == 0 || strcmp("mb", cmd) == 0){
            case_id = CASE_MYBIDS;
            get_message(case_id, command);
        }

        else if(strcmp("show_record", cmd) == 0 || strcmp("sr", cmd) == 0){
            case_id = CASE_SHOW_RECORD;
            get_message(case_id, command);
        }

        else if(strcmp("list", cmd) == 0 || cmd[0] == 'l'){
            case_id = CASE_LIST;
            strcat(command, "LST\n");
        }

        else if(strcmp("exit", cmd) == 0) {
            if (user.logged_in == 1) {
                printf("user not logged out\n");
                continue;
            }
            else break;
        }

        else{
            printf("Invalid command\n");
            continue;
        }
        
        n=sendto(fd, command, strlen(command), 0, res->ai_addr, res->ai_addrlen);
        if(n==-1) /*error*/ exit(1);

        addrlen = sizeof(addr);
        n=recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if(n==-1) /*error*/ exit(1);

        for (int i = 0; buffer[i + CODE_SIZE] != '\n'; i++) {
            reply[i] = buffer[i + CODE_SIZE];
            if (i > 1 && buffer[i + CODE_SIZE]) break;
        }

        if (get_output(case_id, cmd, reply) == 2 && case_id == CASE_LIST) 
            print_auctions(buffer);

        write(1,"answer: ",8); write(1, buffer, n);
        
        //empty command and reply
        memset(command, 0, sizeof(command));
        memset(reply, 0, sizeof(reply));

    }
    freeaddrinfo(res);
    close(fd);
    return 0;
}
