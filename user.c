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
#define NAME_SIZE 10

typedef struct {                                    //struct with info to open auction
    int uid,timeactive, size, aid;                       //5 digitss
    char name[NAME_SIZE];
    char password[PASSWORD_SIZE];
    char asset_fname[NAME_SIZE];
    char data[128];
    char start_value;                              //6 digits
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
    int fd, errcode, uid;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char send_buffer[128], receive_buffer[128], cmd[MAX_CMD_SIZE], password[PASSWORD_SIZE];

    fd=socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errcode=getaddrinfo("tejo.tecnico.ulisboa.pt", PORT, &hints, &res);
    if (errcode != 0) exit(1);

    n=connect(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) exit(1);

    get_word(cmd);
    while(strcmp(cmd,"exit") != 0){
        Open_ open_;
        FILE *fptr;
        if(strcmp(cmd,"open") == 0){                               //open auction
            strcpy(open_.uid,uid);                                //from login
            strcpy(open_.password, password);                     //from login
            get_word(open_.name);
            get_word(open_.asset_fname);
            get_word(open_.start_value);
            get_word(open_.timeactive);

            fptr = fopen(open_.asset_fname,"r");
            if(fptr == NULL)
                printf("Error opening file\n");
            else{
                fseek(fptr, 0, SEEK_END);                         //find size of file
                open_.size = ftell(fptr);
                fseek(fptr, 0, SEEK_SET);                         //find begining of file
                fread(open_.data, 1, open_.size, fptr);
                fclose(fptr);
            }
            strcpy(send_buffer,"OPA");
            strcat(send_buffer," ");
            strcat(send_buffer,open_.uid);
            strcat(send_buffer," ");
            strcat(send_buffer,open_.password);
            strcat(send_buffer," ");
            strcat(send_buffer,open_.name);
            strcat(send_buffer," ");
            strcat(send_buffer,open_.start_value);
            strcat(send_buffer," ");
            strcat(send_buffer,open_.timeactive);
            strcat(send_buffer," ");
            strcat(send_buffer,open_.asset_fname);
            strcat(send_buffer," ");
            strcat(send_buffer,open_.size);
            strcat(send_buffer," ");
            strcat(send_buffer,open_.data);
            strcat(send_buffer,"\n");
    }
    else if(strcmp("close", cmd) == 0){
        get_word(open_.aid);
        strcpy(send_buffer,"CLS");
        strcpy(send_buffer," ");
        strcat(send_buffer,open_.uid);   
        strcat(send_buffer," ");
        strcat(send_buffer,open_.password); 
        strcat(send_buffer," ");
        strcat(send_buffer,open_.aid);       
        strcat(send_buffer,"\n");
    }
    else if(strcmp("show_asset", cmd) == 0 || strcmp("sa", cmd) == 0){
        get_word(open_.aid);
        strcpy(send_buffer,"SAS");
        strcat(send_buffer," ");
        strcat(send_buffer,open_.aid);    
        strcat(send_buffer,"\n");
    }
    else if(strcmp("bid", cmd) == 0 || strcmp("b", cmd) == 0){
        get_word(open_.aid);
        get_word(open_.start_value);
        strcpy(send_buffer,"BID");
        strcat(send_buffer," ");
        strcat(send_buffer,open_.uid); 
        strcat(send_buffer," ");    
        strcat(send_buffer,open_.password);
        strcat(send_buffer," ");
        strcat(send_buffer,open_.aid);  
        strcat(send_buffer," ");
        strcat(send_buffer,open_.start_value);  
        strcat(send_buffer,"\n");        
    }
    
    strcpy(send_buffer,send_buffer);
    n = write(fd, send_buffer, len(send_buffer));
    if (n == -1) exit(1);

    n=read(fd, receive_buffer, 128);
    if (n == -1) exit(1);

    write(1, "answer: ", 8); write(1, receive_buffer, len(receive_buffer));
    }
    freeaddrinfo(res);
    close(fd);

    return 0;
}