#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include "constants.h"
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
    sscanf(buffer, "CLS %d %s %d\n",user.uid, user.password, &auction.aid);
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
            if(auctionIsOwnedByUser(auction.aid, user.uid)==1){ //user owns auction
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

int show_asset(char *buffer){
    Auction auction;
    sscanf(buffer, "SAS %d\n",&auction.aid);
    memset(buffer, 0, 128);
    if(getAssetFileName(auction.aid,auction.asset_fname)!=0){ //can find asset
        auction.size = checkAssetFile(auction.asset_fname);
        getAsset(auction.asset_fname,auction.data);
        sprintf(buffer,"RSA %s %s %ld %s\n",state[2], auction.asset_fname, auction.size, auction.data);
        return 1;
    }
    else{
        sprintf(buffer,"RSA %s\n",state[0]); //cant find asset
        return 0;
    }
}

int bid(char *buffer){
    Auction auction;
    User user;
    int bid;
    sscanf(buffer, "BID %d %s %d %d\n",&user.uid, user.password, &auction.aid, bid);
    memset(buffer, 0, 128);

    if(auctionExists(auction.aid)==-1 || checkActive(auction.aid)!=1){
        sprintf(buffer,"RBD %s\n",state[0]); //auction not active
        return 0;
    }
    else if(checkRegistered(user.uid)==-1 || checkLogin(user.uid)==-1 || checkPassword(user.uid, user.password)!=0){
        sprintf(buffer,"RBD %s\n",state[1]); //user not logged in
        return 0;
    }
    else if(auctionIsOwnedByUser(auction.aid, user.uid)==1){ //check if user owns auction
        sprintf(buffer,"RBD %s\n",state[7]); //bid in own auction
        return 0;
    }
    else if(){ //check if there is a bigger bid
        sprintf(buffer,"RBD %s\n",state[6]); //bid refused
        return 0;
    }
    else{
        sprintf(buffer,"RBD %s\n",state[5]); //bid accepted
        return 1;
    }
}