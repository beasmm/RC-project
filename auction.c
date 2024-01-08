#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>


#include "users.h"
#include "constants.h"

int printdate(Date date){
    printf("%04d-%02d-%02d", date.year, date.month, date.day);
    return 0;
}

int printtime(Time time){
    printf("%02d:%02d:%02d", time.hour, time.minute, time.second);
    return 0;
}

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

    ret = mkdir(aid_dirname, 0777);
    if (ret == -1) return 0;

    sprintf(bids_dirname, "AUCTIONS/%03d/BIDS", aid);

    ret=mkdir(bids_dirname, 0777);
    if (ret == -1) {
        rmdir(aid_dirname);
        return 0;
    }

    sprintf(asset_dirname, "AUCTIONS/%03d/ASSET", aid);

    ret=mkdir(asset_dirname, 0777);
    if (ret == -1) {
        rmdir(aid_dirname);
        rmdir(bids_dirname);
        return 0;
    }
    printf("Auction created\n");
    return 1;
}

int getDetailsFromStartFile(int aid, Auction *auction){
    char path[35];
    FILE *fptr;

    memset(path, 0, 30);
    sprintf(path, "AUCTIONS/%03d/START_%03d.txt", aid, aid);

    fptr = fopen(path, "r");
    if (fptr == NULL) return 0;

    fscanf(fptr, "%s %s %s %d %d %4d-%2d-%2d %2d:%2d:%2d %*s\n", auction->host_uid, auction->name, auction->asset_fname, 
            &auction->start_value, &auction->timeactive, &auction->start_date.year, &auction->start_date.month, 
            &auction->start_date.day, &auction->start_time.hour, &auction->start_time.minute, &auction->start_time.second);

    printf("AUCTIONS name: %s, fname: %s, value: %d, timective: %d, date: ",
            auction->name, auction->asset_fname, auction->start_value, auction->timeactive);
    printdate(auction->start_date);
    printf(", time: ");
    printtime(auction->start_time);
    printf("\n");


    fclose(fptr);
    
    return 1;
}

int createAssetFile(char *filename){
    FILE *fptr;
    fptr = fopen(filename, "w");
    if (fptr == NULL) return 0;
    fclose(fptr);
    return 1;
}

int getAssetFileName(char *aid, char asset_fname[]){ //returns 0 if no asset was found
    char asset_dirname[30];
    char full_path[300];
    DIR *dir;
    struct dirent *ent;
    int i = 0;

    sprintf(asset_dirname, "AUCTIONS/%s/ASSET", aid);

    dir = opendir(asset_dirname);
    if (dir == NULL) return 0;

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        struct stat file_stat;
        snprintf(full_path, sizeof(full_path), "%s/%s", asset_dirname, ent->d_name);
        if (stat(full_path, &file_stat) == 0) {
            if (S_ISREG(file_stat.st_mode)) {
                // If it's a regular file, print its name
                strcpy(asset_fname, ent->d_name);
                i++;
                break;
            }
        } else {
            perror("Error statting file");
        }
    }
    closedir(dir);
    return i;
}

int checkAssetFile(char *asset_fname){
    struct stat filestat;
    int ret_stat;

    ret_stat = stat(asset_fname, &filestat);

    if(ret_stat == -1 || filestat.st_size == 0) return 0;

    return (filestat.st_size);
}

int getAsset(char *asset_fname, char *content){
    FILE *fp;

    int size = checkAssetFile(asset_fname);
    
    fp = fopen(asset_fname, "r");
    if (fp == NULL) return 1;

    fread(content, sizeof(char), size, fp);

    return 0;
}
// returns -1 if auction hasn't been starteds
int auctionExists(int aid){
    char path[35];
    struct stat st = {0};

    sprintf(path, "AUCTIONS/%03d", aid);

    return stat(path, &st);
}

int auctionIsOwnedByUser(int aid, char *uid){
    char path[35];
    struct stat st = {0};

    sprintf(path, "USERS/%s/HOSTED/%03d.txt", uid, aid);

    int ret = stat(path, &st);

    if (ret == -1) return 0;

    return 1;
}

int createStartFile(int aid, char uid[], Auction *auction){
    printf("aid: %03d\n", aid);
    char file_name[35];
    FILE *fptr;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    auction->aid = aid;

    sprintf(file_name, "AUCTIONS/%03d/START_%03d.txt", aid, aid);
    printf("file_name: %s\n", file_name);
    fptr = fopen(file_name, "w");
    if(fptr == NULL) return 0;
    fprintf(fptr, "%s %s %s %d %d %04d-%02d-%02d %02d:%02d:%02d %ld\n", uid, auction->name, auction->asset_fname, 
            auction->start_value, auction->timeactive, tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, 
            tm.tm_min, tm.tm_sec,time(&t));
    return 1;
}

int createBid(int aid, char* uid, int bid){
    char bid_value_file[35], file_name[35];
    FILE *fptr;
    time_t t = time(NULL),numbe ;
    Auction auction;
    User user;
    struct tm tm = *localtime(&t);
    int seconds = 0, se = 0;
    if(strlen(uid) != 6) return 0;
    sprintf(file_name,"AUCTIONS/%d/START_%d.txt", aid, aid);

    fptr = fopen("filename","r");
    fscanf(fptr,"%hhd %s %s %d %d %04d-%02d-%02d %02d:%02d:%02d %d\n", user.uid, auction.name, auction.asset_fname, &auction.start_value, &auction.timeactive, &tm.tm_year + 1900, &tm.tm_mon + 1,&tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec,&se);
    fclose(fptr);

    sprintf(bid_value_file, "AUCTIONS/%s/BIDS/%d.txt", uid, bid);
    fptr = fopen(bid_value_file, "w");
    if(fptr == NULL) return 0;
    
    seconds = time(&t);
    numbe = seconds - se;

    fprintf(fptr, "%s %d %04d-%02d-%02d %02d:%02d:%02d %ld\n",uid, bid, tm.tm_year + 1900, tm.tm_mon + 1,tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, numbe);
    
    fclose(fptr);

    return 1;
}

int writeAuctionData(int aid, char *data){
    char file_path[128];
    char file_name[35];
    FILE *fptr;
    char straid[4];

    printf("aid: %03d\n", aid);
    printf("data: %s\n", data);

    sprintf(straid, "%03d", aid);

    getAssetFileName(straid, file_name);

    sprintf(file_path, "AUCTIONS/%03d/ASSET/%s", aid, file_name);
    fptr = fopen(file_path, "w");
    if(fptr == NULL) return 0;
    fprintf(fptr, "%s", data);
    fclose(fptr);
    return 1;
}


