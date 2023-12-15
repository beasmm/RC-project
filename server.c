#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
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

int main(){
    struct addrinfo hints,*res;
    int fd,newfd,errcode; 
    struct sockaddr_in addr; 
    char *ptr,buffer[128]={0};
    ssize_t n,nw;
    socklen_t addrlen;
    struct sigaction act;

    memset(&act,0,sizeof act);
    act.sa_handler=SIG_IGN;
    if(sigaction(SIGPIPE,&act,NULL)==-1)/*error*/exit(1);


    if((fd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;
    if((errcode=getaddrinfo(NULL,PORT,&hints,&res))!=0)/*error*/exit(1);
    if(bind(fd,res->ai_addr,res->ai_addrlen)==-1)/*error*/{
        perror("[-]Bind error");
        exit(1);
    }
    printf("[+]Bind to the port number: %s\n", PORT);

    if(listen(fd,5)==-1)/*error*/exit(1);
    printf("Listening...\n");

    while(1){
        addrlen=sizeof(addr);
        if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1) exit(1);
        while((n=read(newfd,buffer,128))!=0){if(n==-1)/*error*/exit(1);
            ptr=&buffer[0];
            printf("%s\n",ptr);
            while(n>0){
                if((nw=write(newfd,ptr,n))<=0)/*error*/break;
                n-=1; 
                ptr+=1;
            }
        }
        close(newfd);
    }
    freeaddrinfo(res);
    close(fd);
    return 0;
}