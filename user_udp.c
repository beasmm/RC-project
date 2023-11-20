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
#define MAX_CMD_SIZE 7
#define UID_SIZE 7
#define PASSWORD_SIZE 9

char msg[MAX_CMD_SIZE + UID_SIZE + PASSWORD_SIZE];


typedef struct {
    char code[CODE_SIZE];
    char cmd[MAX_CMD_SIZE];
    char uid[UID_SIZE];
    char password[PASSWORD_SIZE];
} Command;


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

void get_msg(char str[], char code[], char uid[], char password[]){
    strcat(str, code);
    strcat(str, " ");
    strcat(str, uid);
    strcat(str, " ");
    strcat(str, password);
    strcat(str, "\n");
    return;
}


int main(){
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[128];
    //char cmd[MAX_CMD_SIZE], uid[UID_SIZE], password[PASSWORD_SIZE];
    //char command[MAX_CMD_SIZE + UID_SIZE + PASSWORD_SIZE];

    Command command;

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket
    
    errcode=getaddrinfo("tejo.tecnico.ulisboa.pt",PORT,&hints,&res);
    if(errcode!=0) /*error*/ exit(1);

    //scanf("%s %s %s", cmd, uid, password);

    get_word(command.cmd);

    if(strcmp("login", command.cmd) == 0){
        strcpy(command.code, "LIN");
        get_word(command.uid);
        get_word(command.password);
        get_msg(msg, command.code, command.uid, command.password);
    }
    
    else if(strcmp("logout", command.cmd) == 0){
        strcpy(command.code, "LOU");
        get_word(command.uid);
        get_word(command.password);
        get_msg(msg, command.code, command.uid, command.password);
    }


    printf("cmd: %s\n", command.cmd);
    printf("uid: %s\n", command.uid);
    printf("password: %s\n", command.password);


    n=sendto(fd, msg, strlen(msg), 0, res->ai_addr, res->ai_addrlen);
    if(n==-1) /*error*/ exit(1);
    
    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128,0,
        (struct sockaddr*)&addr,&addrlen);
    if(n==-1) /*error*/ exit(1);
    
    write(1,"answer: ",8); write(1, buffer, n);
    
    freeaddrinfo(res);
    close(fd);
}