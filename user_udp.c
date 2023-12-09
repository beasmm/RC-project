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

#define CODE_SIZE 4
#define MAX_CMD_SIZE 11
#define UID_SIZE 7
#define PASSWORD_SIZE 9

#define CASE_LOGIN 0
#define CASE_LOGOUT 1
#define CASE_UNREGISTER 2
#define CASE_MYAUCTIONS 3
#define CASE_MYBIDS 4
#define CASE_SHOW_RECORD 5
#define CASE_LIST 6

typedef struct {
    char uid[UID_SIZE];
    char password[PASSWORD_SIZE];
    int logged_in;
} User;

//global variables
User user;


void get_word(char str[]){
    char c = getchar();
	int i = 0;
	while (c == ' ')
		c = getchar();
	while (c != ' ' && c != '\n') {
		str[i++] = c;
		c = getchar();
	}
	str[i] = '\0';
	return;
}


int login(){
    get_word(user.uid);
    get_word(user.password);
    //TODO: verify arguments
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


int get_output(int case_id, char cmd[], char status[]){
    printf("status: %s\n", status);
    if (strcmp("ERR", status) == 0) {
        printf("server error\n");
        return 0;
    }
    //case login: change logged_in to 1
    if (case_id == CASE_LOGIN) {
        if (strcmp("OK", status) == 0) {
            printf("successful %s\n", cmd);
            user.logged_in = 1;
            return 0;
        }
        else if (strcmp("NOK", status) == 0) {
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
        if (strcmp("OK", status) == 0) {
            printf("successful %s\n", cmd);
            if (case_id == CASE_LOGOUT) user.logged_in = 0; //logout
            return 0;
        }
        else if (strcmp("UNR", status) == 0) {
            printf("unknow user\n");
            return 0;
        }
        //logout 
        else if (case_id == CASE_LOGOUT) {
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
        if (strcmp("NLG", status) == 0) {
            printf("user not logged in\n");
            return 0;
        }
        else if (strcmp("NOK", status) == 0) {
            printf("user has no ongoing bids\n");
            return 0;
        }
        else return 1;
    }
    // case LIST
    else {
        if (strcmp("NOK", status) == 0) {
            printf("no auction started yet\n");
            return 0;
        }
    }
    return 0;
}


void print_auctions(char str[], int offset){
    char aid[3];
    int i = offset, j = 0;
    while(str[i] != '\n') {
        while (j < 3) {
            aid[j++] = str[i++];
            continue;
        }

        if (j == 4) {
            printf("auction_id: %s: ", aid);
            j++;
            i++;
        }
        else if (j == 5) {
            if (str[i] == 'O') printf(" inactive\t");
            else if (str[i] == '1') printf(" active\t");
            j = 0;
            i += 2;
        }
        if (str[i] == '\n') {
            printf("\n");
            return;
        }
    }
    return;
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
    char cmd[MAX_CMD_SIZE], status[CODE_SIZE];
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
            login();
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

        printf("message: %s", command);

        printf("command length: %ld\n", strlen(command));
        
        n=sendto(fd, command, strlen(command), 0, res->ai_addr, res->ai_addrlen);
        if(n==-1) /*error*/ exit(1);
        
        addrlen=sizeof(addr);
        n=recvfrom(fd,buffer,128,0,
            (struct sockaddr*)&addr,&addrlen);
        if(n==-1) /*error*/ exit(1);

        printf("buffer: %s", buffer);

        for (int i = 0; buffer[i + CODE_SIZE] != '\n'; i++) {
            //if (buffer[i + CODE_SIZE] != ' ') break;
            status[i] = buffer[i + CODE_SIZE];
        }


        if (get_output(case_id, cmd, status)) printf("slay\n");
            //print_auctions(buffer, CODE_SIZE + strlen(status));

        printf("status: %s\n", status);

        write(1,"answer: ",8); write(1, buffer, n);
        
        //empty command and status
        memset(command, 0, sizeof(command));
        memset(status, 0, sizeof(status));

    }
    freeaddrinfo(res);
    close(fd);
    return 0;
}

// RLI OK

//hbdjenek
