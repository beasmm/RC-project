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

int open_server(char *buffer, int aid){
    FILE *fptr;
    Auction auction;
    User user;
    char filename[20];
    sscanf(buffer, "OPA %d %s %s %d %d %s %d %s\n",user.uid, user.password, auction.name, &auction.start_value, &auction.timeactive, auction.asset_fname, auction.size, auction.data);
    memset(buffer, 0, 128);
   
    if(checkRegistered(user.uid)==-1 || checkLogin(user.uid)==-1 || checkPassword(user.uid, user.password)!=0){
        sprintf(buffer,"ROA %s\n",state[1]); //user not logged in
        return 0;
    }
    else{
        if(createAuctionDir(aid)==1 && createStartFile(aid, buffer)==1){
            sprintf(filename, "AUCTIONS/%03d/ASSET/%s", aid, auction.asset_fname);
            fptr = fopen(filename, "w");
            if (fptr == NULL) return 0;
            fprintf(fptr,"%s\0", auction.data);
            fclose(fptr);
            sprintf(buffer,"ROA %s %d\n",state[2],aid);
            return 1;
        }
        else{
            sprintf(buffer,"ROA %s\n",state[0]); //auction could not be started
            return 0;
        }
    }
}

int close_server(char *buffer){
    FILE *fptr;
    Auction auction;
    User user;
    time_t t = time(NULL),numbe ;
    struct tm tm = *localtime(&t);
    char filename[20];
    int seconds = 0, se = 0;
    sscanf(buffer, "CLS %s %s %d\n",user.uid, user.password, &auction.aid);
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
                sprintf(filename,"AUCTIONS/%d/START_%d.txt", aid, aid);

                fptr = fopen(filename,"r");
                fscanf(fptr,"%d %s %s %d %d %04d-%02d-%02d %02d:%02d:%02d %d\n", user.uid, auction.name, auction.asset_fname, auction.start_value, auction.timeactive, tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,se);
                fclose(fptr);

                seconds = time(&t);
                numbe = seconds - se;
                memset(filename,0,20);

                sprintf(filename, "AUCTIONS/%03d/END_%d", auction.aid, auction.aid);
                fptr = fopen(filename, "w");
                if (fptr == NULL) {
                    sprintf(buffer,"RCL %s\n",state[8]); // error deleting auction
                    return 0;
                }
                else{
                fprintf(fptr, fprintf(fptr, "%04d-%02d-%02d %02d:%02d:%02d %d\n",tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,numbe));
                fprintf(fptr,"%s\0", auction.data);
                fclose(fptr);

                sprintf(buffer,"RCL %s\n",state[2]); // auction closed
                return 1;
                }
            }
            else{
                sprintf(buffer,"RCL %s\n",state[9]); // user doesnt own auction
                return 0;
            }
        }
    }
}

int show_asset_server(char *buffer){
    Auction auction;
    sscanf(buffer, "SAS %d\n",&auction.aid);
    memset(buffer, 0, 128);
    char straid[4] = {0};
    sprintf(straid,"%03d",auction.aid);
    if(getAssetFileName(straid, auction.asset_fname)!=0){ //can find asset
        auction.size = checkAssetFile(straid);
        getAsset(auction.asset_fname,auction.data);
        sprintf(buffer,"RSA %s %s %ld %s\n",state[2], auction.asset_fname, auction.size, auction.data);
        return 1;
    }
    else{
        sprintf(buffer,"RSA %s\n",state[0]); //cant find asset
        return 0;
    }
}

int bid_server(char *buffer){
    Auction auction;
    User user;
    int bid;
    sscanf(buffer, "BID %s %s %d %d\n", user.uid, user.password, &auction.aid, &bid);
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
    else if(1){ //check if there is a bigger bid
        sprintf(buffer,"RBD %s\n",state[6]); //bid refused
        return 0;
    }
    else{
        createBid(auction.aid,user.uid,bid);
        sprintf(buffer,"RBD %s\n",state[5]); //bid accepted
        return 1;
    }
}