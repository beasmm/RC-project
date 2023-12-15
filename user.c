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
#include <signal.h>
#define PORT "58011"

#define MAX_CMD_SIZE 7
#define UID_SIZE 7
#define PASSWORD_SIZE 9
#define FNAME 24
#define FSIZE 8

int fd, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char cmd[MAX_CMD_SIZE]={0};

typedef struct{
    char uid[UID_SIZE];
    char password[PASSWORD_SIZE];
} User_;

typedef struct {      
    char name[15];
    char asset_fname[FNAME];
    char start_value[15];
    char timeactive[5];
    char size[FSIZE];
    char data[15];
    char aid[3];
    char value[15];
    User_ user;
} Open_;

int main(){
    int size;
    Open_ open;
    FILE *fptr;
    int open_state;
    int login_state;
    char code[3]={0};
    char status[3]={0};
    char aid[3]={0};
    struct sigaction act;

    memset(&act,0,sizeof act);
    act.sa_handler=SIG_IGN;

    if(sigaction(SIGPIPE,&act,NULL)==-1) exit(-1);

    printf("Introduce action code: ");
    scanf("%s", cmd);

    while(strcmp("exit",cmd)!=0){
        fd = socket(AF_INET, SOCK_STREAM,0);
        if (fd==-1) exit(1);
        memset(&hints,0,sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype=SOCK_STREAM;

        errcode=getaddrinfo("127.0.0.1",PORT,&hints,&res);
        if(errcode!=0) exit(1);

        n=connect(fd,res->ai_addr,res->ai_addrlen);
        if(n==-1) exit(1);


        char buffer[128]={0};
        char buffer_to_send[128]={0};
        if (strcmp("open",cmd)==0){
            strcpy(open.user.uid,"103029");
            strcpy(open.user.password,"123456AA");
            scanf("%s %s %s %s",open.name,open.asset_fname,open.start_value,open.timeactive);

            fptr = fopen(open.asset_fname,"r");
            if(fptr == NULL){
                printf("Error opening file\n");                    //??
                exit(1);
            }
            else{
                fseek(fptr, 0, SEEK_END);                         //find size of file
                sprintf(open.size,"%ld",ftell(fptr)+1);
                size = ftell(fptr)+1;
                fseek(fptr, 0, SEEK_SET);                         //find begining of file
                fread(open.data, size, 1,fptr);
                fclose(fptr);
            }
            sprintf(buffer_to_send,"OPA %s %s %s %s %s %s %s %s\n",open.user.uid,open.user.password,open.name,open.start_value,open.timeactive,open.asset_fname,open.size,open.data);

            n=write(fd,buffer_to_send,128);
            if(n==-1) exit(1);
            
            n=read(fd,buffer,128);
            if(n==-1) exit(1);
            
            write(1,"echo: ",6); write(1,buffer,n);
            memcpy(code,&buffer[0],3*sizeof(char));
            status[3] = '\0';

            memcpy(status,&buffer[4],3*sizeof(char));
            status[3] = '\0';
            if(strcmp(status,"NLG")==0){
                printf("User not logged in\n");
            }
            freeaddrinfo(res);
            close(fd);
            open_state = 0;
            printf("Introduce action code: ");
            scanf("%s", cmd);
        }
        else if(strcmp("close",cmd)==0){ //cancelling the sale, or if the auction time had already ended
            if(open_state==0){
                scanf("%s",open.aid);
                sprintf(buffer_to_send,"CLS %s %s %s\n",open.user.uid,open.user.password,open.aid);
                
                n=write(fd,buffer_to_send,128);
                if(n==-1) exit(1);

                n=read(fd,buffer,128);
                if(n==-1) exit(1);

                write(1,"echo: ",6); write(1,buffer,n);

                memcpy(code,&buffer[0],3*sizeof(char));
                code[3] = '\0';

                memcpy(status,&buffer[4],3*sizeof(char));
                status[3] = '\0';
            
                if (strcmp(status,"EAU")==0){
                    printf("Auction %s does not exist\n", open.aid);
                }
                else if(strcmp(status,"NLG")==0){
                    printf("User not logged in\n");
                }
                else if(strcmp(status,"EOW")==0){
                    printf("Auction %s is not owned by user %s\n",open.aid,open.user.uid);
                }
                else if(strcmp(status,"END")==0){
                    printf("Auction %s owned by user %s has already finished\n", open.aid,open.user.uid);
                }
                else if(strcmp(status,"ERR")==0){
                    printf("Error\n");
                }
                else{
                    printf("Auction %s closed\n", open.aid);
                }

                freeaddrinfo(res);
                close(fd);
            }
            else{
                printf("Can't find auction to close\n");
            }
            printf("Introduce action code: ");
            scanf("%s", cmd);
        }
        else if(strcmp(cmd,"show_asset")==0 || strcmp(cmd,"sa")==0){
            scanf("%s",open.aid);
            sprintf(buffer_to_send,"SAS %s\n",open.aid);

            n=write(fd,buffer_to_send,128);
            if(n==-1) exit(1);

            n=read(fd,buffer,128);
            if(n==-1) exit(1);

            write(1,"echo: ",6); write(1,buffer,n);printf("\n");

            memcpy(code,&buffer[0],3);
            code[4] = '\0';
            memcpy(status,&buffer[4],7);
            status[3] = '\0';

            if (strcmp(status,"NOK")==0){
                printf("Auction could not be started\n");
            }
            else if(strcmp(status,"NLG")==0){
                printf("User not logged in\n");
            }
            else if(strcmp(status,"ERR")==0){
                printf("Error\n");
            }
            else{
                memcpy(aid,&buffer[7],10);
                aid[3] = '\0';
                printf("Auction number %s started\n", open.aid);   //FALTA PARTE
            }
            freeaddrinfo(res);
            close(fd);
            printf("Introduce action code: ");
            scanf("%s", cmd);
        }
        else if(strcmp(cmd,"bid")==0 || strcmp(cmd,"b")==0){
            scanf("%s",open.aid);
            scanf("%s",open.value);
            sprintf(buffer_to_send,"BID %s %s %s %s\n",open.user.uid,open.user.password,open.aid,open.value);

            n=write(fd,buffer_to_send,128);
            if(n==-1) exit(1);

            n=read(fd,buffer,128);
            if(n==-1) exit(1);

            write(1,"echo: ",6); write(1,buffer,n);

            memcpy(code,&buffer[0],3);
            code[4] = '\0';
            memcpy(status,&buffer[4],7);
            status[3] = '\0';

            if (strcmp(status,"NOK")==0){
                printf("Auction %s is not active\n",open.aid);
            }
            else if(strcmp(status,"NLG")==0){
                printf("User not logged in\n");
            }
            else if(strcmp(status,"ACC")==0){
                printf("Bid was accepted\n");
            }
            else if(strcmp(status,"REF")==0){
                printf("Bid refused - larger bid was already been placed\n");
            }
            else if(strcmp(status,"ILG")==0){
                printf("User tried to bid in auction hosted by himself\n");
            }
            else if(strcmp(status,"ERR")==0){
                printf("Error\n");
            }

            freeaddrinfo(res);
            close(fd);

            printf("Introduce action code: ");
            scanf("%s", cmd);
        }
    }
    return 0;
 }