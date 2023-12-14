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

int fd, newfd, errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[128];

int main(){

    fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd == -1) exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;

    errcode=getaddrinfo(NULL,PORT,&hints,&res);
    if((errcode)!=0) exit(1);

    n=bind(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1){
        perror("bind");
        exit(1);
    }

    if(listen(fd,5)==-1) exit(1);
    while(1){
        addrlen=sizeof(addr);
        if((newfd=accept(fd,(struct sockaddr*) &addr, &addrlen))==-1) exit(1);
        printf("connected\n");
        n=read(newfd,buffer,128);
        if(n==-1) exit(1);
        write(1,"received: ",10); write(1,buffer,n);

        n=write(newfd,buffer,n);
        if(n==-1) exit(1);

        close(newfd);
    }
    freeaddrinfo(res);
    close(fd);
    return 0;
}