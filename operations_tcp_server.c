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
   
    //verificar uid e password
    if(checkRegistered(user.uid)==-1 || checkLogin(user.uid)==-1 || checkPassword(user.uid, user.password)!=0){
        sprintf(buffer,"ROA %s\n",state[1]); //user not logged in
        return 0;
    }
    else{
        if(createAuctionDir(aid)==1){
            //guardar file localmente
            sprintf(buffer,"ROA %s %d\n",state[2],aid);
            return 1;
        }
        else{
            sprintf(buffer,"ROA %s\n",state[0]); //auction could not be started
            return 0;
        }
    }
}

int close(char *buffer){
    Auction auction;
    User user;
    sscanf(buffer, "CLS %d %s %d\n",user.uid, user.password, auction.aid);
    memset(buffer, 0, 128);
    if(auctionExists(auction.aid)==-1){
        sprintf(buffer,"RCL %s\n",state[3]); // auction doesnt exist
        return 0;
    }
    else{
        if(checkActive(auction.aid)!=1){
            sprintf(buffer,"RCL %s\n",state[4]); // auction has already finished
            return 0;
        }
        else{
            if(){ //user owns auction
                //create file on closed auctions
                if(){ //delete auction
                    sprintf(buffer,"RCL %s\n",state[2]); // auction closed
                    return 1;
                }
                else{
                    sprintf(buffer,"RCL %s\n",state[8]); // error deleting auction
                    return 0;
                }
            }
            else{
                sprintf(buffer,"RCL %s\n",state[9]); // user doesnt own auction
                return 0;
            }
        }
    }
}
