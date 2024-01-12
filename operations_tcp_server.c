#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>

#include "users.h"
#include "auction.h"

int open_server(char *buffer, int aid, char file_name[]){
    //Verify user details
    Auction auction;
    User user;
    char filename[50];
    sscanf(buffer, "OPA %s %s %s %d %d %s %ld\n",user.uid, user.password, auction.name, &auction.start_value, &auction.timeactive, auction.asset_fname, &auction.size);
    memset(buffer, 0, 128);

    if(checkRegistered(user.uid) == -1 || checkLogin(user.uid) == -1 || checkPassword(user.uid, user.password) != 0){
        sprintf(buffer, "ROA NLG\n"); //user not logged in
        return 0;
    }
    else{
        if(createAuctionDir(aid) == 1 && createStartFile(aid, user.uid, &auction) == 1 && addToHosted(aid, user.uid)){
            sprintf(filename, "AUCTIONS/%03d/ASSET/%s", aid, auction.asset_fname);
            createAssetFile(filename);
            sprintf(buffer, "ROA OK %d\n", aid);
            strcpy(file_name, filename);
            return auction.size;
        }
        else{
            sprintf(buffer, "ROA NOK\n"); //auction could not be started
            return 0;
        }
    }
}

int close_server(char *buffer){
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
            sprintf(buffer, "RCL OK\n"); // auction closed
            createENDFile(aid);
            return 1;
        } 
        else {
            sprintf(buffer, "RCL EOW\n"); // user doesnt own auction
            return 0;
        }
    }
}

int show_asset_server(char *buffer, char file_name[]){
    Auction auction;
    sscanf(buffer, "SAS %d\n",&auction.aid);
    memset(buffer, 0, 128);
    if(getAssetFileName(auction.aid, auction.asset_fname) != 0){ //can find asset
        char path[50];
        sprintf(path, "AUCTIONS/%03d/ASSET/%s", auction.aid, auction.asset_fname);
        auction.size = checkAssetFile(path);
        sprintf(buffer, "RSA OK %s %ld\n", auction.asset_fname, auction.size);
        strcpy(file_name, path);
        return auction.size;
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

//sending and transfering files
int server_sendFile(int fd, char* filename, int size){
    FILE *file = fopen(filename, "rb");
    if (file == NULL) return -1;

    int filefd = fileno(file);
    size_t total_sent = 0;
    size_t n_sent, n_read, done;
    char data[MAXLINE];

    // Read data from the file
    while (total_sent < size) {
        memset(data, 0, MAXLINE);
        n_read = read(filefd, data, sizeof(data) - 1);
        done = 0;
        while (done < n_read) {
            n_sent = write(fd, data + done, n_read - done);
            if (n_sent == -1) {
                perror("Error sending file");
                fclose(file);
                return -1;
            }
            done += n_sent;
        }
        total_sent += n_read;
    }
    fclose(file);
    return 0;
}

int server_receiveFile(int fd, char *filename, int n){
    FILE *file = fopen(filename, "wb");
    if (file == NULL){
        perror("Error opening file");
        return -1;
    }
    
    size_t total = 0;
    size_t n_read, bytesRead, done;

    char data[MAXLINE];
    memset(data, 0, MAXLINE);

    while (total < n) {
        bytesRead = read(fd, data, sizeof(data));
        done = 0;
        while (done < bytesRead) {
            n_read = fwrite(data, 1, bytesRead, file);
            if (n_read == -1) {
                perror("Error writing file");
                fclose(file);
                return -1;
            }
            done += n_read;
        }
        total += done;
    }
    fclose(file);
    return 0;
}