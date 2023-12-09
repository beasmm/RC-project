#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#define PORT "58011"

#define MAX_CMD_SIZE 7
#define UID_SIZE 7
#define PASSWORD_SIZE 9

int fd, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char cmd[MAX_CMD_SIZE];

typedef struct{
    char uid[UID_SIZE];
    char password[PASSWORD_SIZE];
} User_;

typedef struct {      
    char name[15];
    char asset_fname[15];
    char start_value[15];
    char timeactive[15];
    char size[15];
    char data[128];
    char aid[15];
    char value[15];
    User_ user;
} Open_;

int main(){
    int size;
    Open_ open;
    FILE *fptr;
    int open_state;
    int login_state;
    fd = socket(AF_INET, SOCK_STREAM,0);
    if (fd==-1) exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype=SOCK_STREAM;

    errcode=getaddrinfo("tejo.tecnico.ulisboa.pt",PORT,&hints,&res);
    if(errcode!=0) exit(1);

    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1) exit(1);

    scanf("%s", cmd);
    while(strcmp("exit",cmd)!=0){
        char buffer[128];
        char buffer_to_send[128];
        if (strcmp("open",cmd)==0){
            strcpy(open.user.uid,"103029");
            strcpy(open.user.password,"123456AA");
            scanf("%s",open.name);
            scanf("%s",open.asset_fname);
            scanf("%s",open.start_value);
            scanf("%s",open.timeactive);

            fptr = fopen(open.asset_fname,"r");
            if(fptr == NULL)
                printf("Error opening file\n");
            else{
                fseek(fptr, 0, SEEK_END);                         //find size of file
                sprintf(open.size,"%ld",ftell(fptr)+1);
                size = ftell(fptr)+1;
                fseek(fptr, 0, SEEK_SET);                         //find begining of file
                fread(open.data, size, 1,fptr);
                fclose(fptr);
            }

            strcpy(buffer_to_send,"OPA");
            strcat(buffer_to_send,open.user.uid);
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send,open.user.password);
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send, open.name);
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send, open.start_value);
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send, open.timeactive);
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send, open.asset_fname);
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send, open.size);
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send, open.data);
            strcat(buffer_to_send, "\n");

            n=write(fd,buffer_to_send,128);
            if(n==-1) exit(1);

            n=read(fd,buffer,128);
            if(n==-1) exit(1);

            write(1,"echo: ",6); write(1,buffer,n);
            freeaddrinfo(res);
            close(fd);
            open_state = 0;
            scanf("%s",cmd);
        }
        else if(strcmp("close",cmd)==0){ //cancelling the sale, or if the auction time had already ended
            if(open_state==0){
                scanf("%s",open.aid);
                strcpy(buffer_to_send,"CLS");
                strcat(buffer_to_send, " ");
                strcat(buffer_to_send, open.user.uid);
                strcat(buffer_to_send, " ");
                strcat(buffer_to_send, open.user.password);
                strcat(buffer_to_send, " ");
                strcat(buffer_to_send, open.aid);
                strcat(buffer_to_send, "\n");
                n=write(fd,buffer_to_send,128);
                if(n==-1) exit(1);

                n=read(fd,buffer,128);
                if(n==-1) exit(1);

                write(1,"echo: ",6); write(1,buffer,n);
                freeaddrinfo(res);
                close(fd);
                scanf("%s",cmd);
            }
            else{
                printf("Use open before close\n");
                scanf("%s",cmd);
            }
        }
        else if(strcmp(cmd,"show_asset")==0 || strcmp(cmd,"sa")==0){
            scanf("%s",open.aid);
            strcpy(buffer_to_send,"SAS");
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send,open.aid);
            strcat(buffer_to_send,"\n");

            n=write(fd,buffer_to_send,128);
            if(n==-1) exit(1);

            n=read(fd,buffer,128);
            if(n==-1) exit(1);

            write(1,"echo: ",6); write(1,buffer,n);
            freeaddrinfo(res);
            close(fd);
            scanf("%s",cmd);
        }
        else if(strcmp(cmd,"bid")==0 || strcmp(cmd,"b")){
            scanf("%s",open.aid);
            scanf("%s",open.value);
            strcpy(buffer_to_send,"BID");
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send, open.user.uid);
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send, open.user.password);
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send, open.aid);
            strcat(buffer_to_send, " ");
            strcat(buffer_to_send, open.value);
            strcat(buffer_to_send, "\n");

            n=write(fd,buffer_to_send,128);
            if(n==-1) exit(1);

            n=read(fd,buffer,128);
            if(n==-1) exit(1);

            write(1,"echo: ",6); write(1,buffer,n);

            freeaddrinfo(res);
            close(fd);

            scanf("%s",cmd);
        }
    }
    return 0;
 }