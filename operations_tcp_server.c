#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include "constants_tcp.h"
#include "users.h"
#include "auction.h"

char state[9][3]={"NOK","NLG","OK","EAU","END","ACC","REF","ILG","ERR"};
int aid;

int open(char *buffer){
    FILE *fptr;
    Auction auction;
    User user;
    sscanf(buffer, "OPA %d %s %s %d %d %s %ld %s\n",user.uid, user.password, auction.name,&auction.start_value, &auction.timeactive, auction.asset_fname);
    memset(buffer, 0, 128);
    fptr = fopen(auction.asset_fname,"r");
    if(fptr == NULL){
        sprintf(buffer,"ROA %s\n",state[0]);
        return 0;
    }
    else{
        fseek(fptr, 0, SEEK_END);                         //find size of file
        auction.size = ftell(fptr)+1;
        fseek(fptr, 0, SEEK_SET);                         //find begining of file
        fread(open.data, auction.size, 1,fptr);
        fclose(fptr);
    }
    //verificar uid e password
    sprintf(buffer,"ROA %s\n",state[1]); //user not logged in
    //abrir file
    //guardar file localmente
    sprintf(buffer,"ROA %s %d\n",state[2],aid);  //aid
    return 1;
}

int close(char *buffer){
    Auction auction;
    sscanf(buffer, "CLS %d %s %d\n",auction.)
}
