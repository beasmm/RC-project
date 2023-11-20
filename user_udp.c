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

void get_msg(int type, char str[]){
    char codes[6][CODE_SIZE] = {"LIN", "LOU", "UNR", "LMA", "LMB", "LST"};
    char uid[UID_SIZE];

    if (type < 3) {
        char password[PASSWORD_SIZE];
        get_word(uid);
        get_word(password);
        strcat(str, codes[type]);
        strcat(str, " ");
        strcat(str, uid);
        strcat(str, " ");
        strcat(str, password);
        strcat(str, "\n");
    }
    else if (type < 5) {
        get_word(uid);
        strcat(str, codes[type]);
        strcat(str, " ");
        strcat(str, uid);
        strcat(str, "\n");
    }
    else {
        strcat(str, codes[type]);
        strcat(str, "\n");
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

    char cmd[MAX_CMD_SIZE];
    char command[CODE_SIZE + UID_SIZE + PASSWORD_SIZE];

    fd=socket(AF_INET,SOCK_DGRAM,0); //UDP socket
    if(fd==-1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket
    
    errcode=getaddrinfo("tejo.tecnico.ulisboa.pt",PORT,&hints,&res);
    if(errcode!=0) /*error*/ exit(1);

    //scanf("%s %s %s", cmd, uid, password);

    get_word(cmd);

    if(strcmp("login", cmd) == 0){
        get_msg(0, command);
    }
    
    else if(strcmp("logout", cmd) == 0){
        get_msg(1, command);
    }

    else if(strcmp("unregister", cmd) == 0){
        get_msg(2, command);
    }

    else if(strcmp("myauctions", cmd) == 0 || strcmp("ma", cmd) == 0){
        get_msg(3, command);
    }

    else if(strcmp("mybids", cmd) == 0 || strcmp("mb", cmd) == 0){
        get_msg(4, command);
    }

    else if(strcmp("list", cmd) == 0 || cmd[0] == 'l'){
        get_msg(5, command);
    }

    printf("message: %s", command);


    n=sendto(fd, command, strlen(command), 0, res->ai_addr, res->ai_addrlen);
    if(n==-1) /*error*/ exit(1);
    
    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128,0,
        (struct sockaddr*)&addr,&addrlen);
    if(n==-1) /*error*/ exit(1);
    
    write(1,"answer: ",8); write(1, buffer, n);
    
    freeaddrinfo(res);
    close(fd);

    return 0;
}

