#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#define PORT "58011"

#define CODE_SIZE 4
#define MAX_CMD_SIZE 7
#define UID_SIZE 7
#define PASSWORD_SIZE 9
#define NAME_SIZE 15

typedef struct {   //struct with info to open auction
    char code[MAX_CMD_SIZE];
    char cmd[MAX_CMD_SIZE];
    int uid;
    char name[NAME_SIZE];
    char password[PASSWORD_SIZE];
    char asset_fname[NAME_SIZE];
    float start_value;
    float timeactive;
} Open_;

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

int main(){
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    char msg[17];
    char cmd[MAX_CMD_SIZE];
    int uid;
    char password[PASSWORD_SIZE];

    fd=socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errcode=getaddrinfo("tejo.tecnico.ulisboa.pt", PORT, &hints, &res);
    if (errcode != 0) exit(1);

    n=connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) exit(1);

    get_word(cmd); //get command

    if(strcmp("open", cmd) == 0){ //open auction
        Open_ open_;
        strcpy(open_.cmd, cmd);
        strcpy(open_.code, "OPA");
        strcpy(open_.uid,uid);
        strcpy(open_.password, password);
        get_word(open_.name);
        get_word(open_.asset_fname);
        get_word(open_.timeactive);
        FILE* asset_image_file;
        asset_image_file = fopen(open_.asset_fname,"r"); //verificar se foi aberto
        
    }

    n = write(fd, msg, 21);
    if (n == -1) exit(1);

    n=read(fd, buffer, 128);
    if (n == -1) exit(1);

    write(1, "answer: ", 8); write(1, buffer, n);

    freeaddrinfo(res);
    close(fd);

    return 0;
}

