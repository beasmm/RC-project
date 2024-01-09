#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

#include "users.h"
#include "auction.h"

char state[10][3]={"NOK", "NLG", "OK", "EAU", "END", "ACC", "REF", "ILG", "ERR", "EOW"};

int open_server(char *buffer, int aid){
    //Verify user details
    printf("entering open\n");
    Auction auction;
    User user;
    char filename[50];
    sscanf(buffer, "OPA %s %s %s %d %d %s %ld \n",user.uid, user.password, auction.name, &auction.start_value, &auction.timeactive, auction.asset_fname, &auction.size);
    memset(buffer, 0, 128);

    printf("auction.name: %s\n", auction.name);
    printf("auction.start_value: %d\n", auction.start_value);
    printf("auction.timeactive: %d\n", auction.timeactive);
    printf("auction.asset_fname: %s\n", auction.asset_fname);
   
    if(checkRegistered(user.uid)==-1 || checkLogin(user.uid)==-1 || checkPassword(user.uid, user.password)!=0){
        sprintf(buffer, "ROA NLG\n"); //user not logged in
        return 0;
    }
    else{
        if(createAuctionDir(aid) == 1 && createStartFile(aid, user.uid, &auction)==1 && addToHosted(aid, user.uid)){
            sprintf(filename, "AUCTIONS/%03d/ASSET/%s", aid, auction.asset_fname);
            createAssetFile(filename);
            sprintf(buffer, "ROA OK %d\n", aid);
            return auction.size;
        }
        else{
            sprintf(buffer, "ROA NOK\n"); //auction could not be started
            return 0;
        }
    }
}

int close_server(char *buffer){
    FILE *fptr;
    int aid;
    User user;

    sscanf(buffer, "CLS %s %s %d\n", user.uid, user.password, &aid);
    memset(buffer, 0, 128);

    if(auctionExists(aid) == -1){
        sprintf(buffer, "RCL EAU\n"); // auction doesnt exist
        return 0;
    }
    else if(checkRegistered(user.uid) != 0 || checkLogin(user.uid) != 0 || checkPassword(user.uid, user.password) != 0){
        sprintf(buffer, "RCL NOK\n"); //invalid user
        return 0;
    }
    else{
        if(auctionIsOwnedByUser(aid, user.uid)){ //user owns auction
            if(!checkActive(aid)){
                sprintf(buffer, "RCL END\n"); // auction has already finished
                return 0;
            }
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            char filename[50];
            int current_time, start_time, time_active;

            memset(filename, 0, 50);
            sprintf(filename, "AUCTIONS/%03d/START_%03d.txt", aid, aid);
            fptr = fopen(filename, "r");
            fscanf(fptr, "%*s %*s %*s %*d %*d %*d-%*d-%*d %*d:%*d:%*d %d\n", &start_time);
            fclose(fptr);

            current_time = time(&t);
            time_active = current_time - start_time;
            
            memset(filename, 0, 50);
            sprintf(filename, "AUCTIONS/%03d/END_%03d.txt", aid, aid);
            fptr = fopen(filename, "w");
            if (fptr == NULL) {
                sprintf(buffer, "ERR\n"); // error closing auction
                return 0;
            }
            fprintf(fptr, "%04d-%02d-%02d %02d:%02d:%02d %d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, time_active);
            fclose(fptr);

            sprintf(buffer, "RCL OK\n"); // auction closed
            return 1;
        } 
        else {
            sprintf(buffer, "RCL EOW\n"); // user doesnt own auction
            return 0;
        }
    }
}

int show_asset_server(char *buffer){
    Auction auction;
    sscanf(buffer, "SAS %d\n",&auction.aid);
    memset(buffer, 0, 128);
    char straid[4] = {0};
    sprintf(straid, "%03d",auction.aid);
    if(getAssetFileName(straid, auction.asset_fname)!=0){ //can find asset
        auction.size = checkAssetFile(straid);
        getAsset(auction.asset_fname,auction.data);
        sprintf(buffer, "RSA %s %s %ld %s\n",state[2], auction.asset_fname, auction.size, auction.data);
        return 1;
    }
    else{
        sprintf(buffer, "RSA NOK\n"); //cant find asset
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
        sprintf(buffer, "RBD NOK\n"); //auction not active
        return 0;
    }
    else if(checkRegistered(user.uid)==-1 || checkLogin(user.uid)==-1 || checkPassword(user.uid, user.password)!=0){
        sprintf(buffer, "RBD NLG\n"); //user not logged in
        return 0;
    }
    else if(auctionIsOwnedByUser(auction.aid, user.uid)==1){ //check if user owns auction
        sprintf(buffer, "RBD ILG\n"); //bid in own auction
        return 0;
    }
    else if(!checkBidAmmount(auction.aid, bid)){ //check if there is a bigger bid
        sprintf(buffer, "RBD REF\n"); //bid refused
        return 0;
    }
    else{
        if(createBid(auction.aid, user.uid, bid) == 0) printf("error creating bid\n");
        sprintf(buffer, "RBD ACC\n"); //bid accepted
        return 1;
    }
}