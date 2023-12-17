#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include "constants_tcp.h"
#include "constants_udp.h"
#include "users.h"
#include "auction.h"

char state[10][3]={"NOK","NLG","OK","EAU","END","ACC","REF","ILG","ERR","EOW"};
int aid;

int open(char *buffer){
    FILE *fptr;
    Auction auction;
    User user;
    sscanf(buffer, "OPA %d %s %s %d %d %s %ld %s\n",user.uid, user.password, auction.name,&auction.start_value, &auction.timeactive, auction.asset_fname);
    memset(buffer, 0, 128);
   
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
    User user;
    sscanf(buffer, "CLS %d %s %d\n",user.uid, user.password, auction.aid);
    memset(buffer, 0, 128);
    if(checkActive(auction.aid)==1){ //vai fechar

    }
    else{
        sprintf(buffer,"RCL %s\n",state[4]); // auction has already finished
    }
    sprintf(buffer,"RCL %s\n",state[3]); // auction doesnt exist
    sprintf(buffer,"RCL %s\n",state[9]); // auction is not owned by user
    sprintf(buffer,"RCL %s\n",state[4]); // auction has already finished
    sprintf(buffer,"RCL %s\n",state[2]); // auction closed
}
