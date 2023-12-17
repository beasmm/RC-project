#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>


#include "users.h"

int initAuctions(){
    struct stat st = {0};
    int ret;

    if (stat("AUCTIONS", &st) == -1) {
        ret = mkdir("AUCTIONS", 0777);
        if (ret == -1) {
            printf("failed to create AUCTIONS directory\n");
            return 0;
        }
    }
    return 0;
}

// returns 1 if auction is active
int checkActive(int aid){
    char aid_dirname[30];
    memset(aid_dirname, 0, 30);
    sprintf(aid_dirname, "AUCTIONS/%03d/END_%03d.txt", aid, aid);

    return access(aid_dirname, F_OK) + 1;
}

int createAuctionDir(int aid){
    char aid_dirname[15];
    char bids_dirname[20];
    char asset_dirname[20];
    int ret;

    if (aid < 1 || aid > 999) return 0;

    sprintf(aid_dirname, "AUCTIONS/%03d", aid);

    ret = mkdir(aid_dirname, 0700);
    if (ret == -1) return 0;

    sprintf(bids_dirname, "AUCTIONS/%03d/BIDS", aid);

    ret=mkdir(bids_dirname, 0700);
    if (ret == -1) {
        rmdir(aid_dirname);
        return 0;
    }

    sprintf(asset_dirname, "AUCTIONS/%03d/ASSET", aid);

    ret=mkdir(asset_dirname, 0700);
    if (ret == -1) {
        rmdir(aid_dirname);
        rmdir(bids_dirname);
        return 0;
    }
    return 1;
}

int checkAssetFile(char *asset_fname){
    struct stat filestat;
    int ret_stat;

    ret_stat = stat(asset_fname, &filestat);

    if(ret_stat == -1 || filestat.st_size == 0) return 0;

    return (filestat.st_size);
}

// returns -1 if auction hasn't been starteds
int auctionExists(int aid){
    char path[35];
    struct stat st = {0};

    sprintf(path, "AUCTIONS/%03d", aid);

    return stat(path, &st);
}