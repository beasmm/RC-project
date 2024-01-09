#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


#include "users.h"
#include "constants.h"

//returns the last user id
int getAuctionID(){
    char path[300];
    DIR *dir;
    struct dirent *ent;
    int i = 0;

    dir = opendir("AUCTIONS");
    if (dir == NULL) return 0;

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        struct stat file_stat;
        sprintf(path, "AUCTIONS/%s", ent->d_name);
        if (stat(path, &file_stat) == 0) {
            if (S_ISDIR(file_stat.st_mode)) {
                // If it's a directory, print its name
                i++;
            }
        } else {
            perror("Error statting file");
        }
    }
    closedir(dir);
    return i+1;
}

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

    if (access(aid_dirname, F_OK) == 0) return 0;
    
    else return 1;
}

int createAuctionDir(int aid){
    char aid_dirname[15];
    char bids_dirname[20];
    char asset_dirname[20];
    int ret;

    printf("aid: %03d\n", aid);

    if (aid < 1 || aid > 999) return 0;

    sprintf(aid_dirname, "AUCTIONS/%03d", aid);

    ret = mkdir(aid_dirname, 0777);
    if (ret == -1) {
        printf("mkdir1 failed\n");
        return 0;
    }

    sprintf(bids_dirname, "AUCTIONS/%03d/BIDS", aid);

    ret=mkdir(bids_dirname, 0777);
    if (ret == -1) {
        printf("mkdir2 failed\n");
        rmdir(aid_dirname);
        return 0;
    }

    sprintf(asset_dirname, "AUCTIONS/%03d/ASSET", aid);

    ret=mkdir(asset_dirname, 0777);
    if (ret == -1) {
        printf("mkdir3 failed\n");
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
    fclose(fp);

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
    fclose(fptr);
    return 1;
}

int getBiggestBid(int aid){ //TODO: chech fname and path sizes no enunciado
    char path[30];
    sprintf(path, "AUCTIONS/%03d/BIDS", aid);

    DIR *dir = opendir(path);
    if (dir == NULL) return -1;

    strcat(path, "/");

    struct dirent *ent;
    int biggest_bid = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        int bid = atoi(ent->d_name);
        if (bid > biggest_bid) biggest_bid = bid;
    }
    printf("biggest_bid: %d\n", biggest_bid);
    return biggest_bid;
}

int checkBidAmmount(int aid, int bid){
    char file_name[35];
    FILE *fptr;
    int min_bid;

    sprintf(file_name, "AUCTIONS/%03d/BIDS", aid);

    if(isDirectoryEmpty(file_name)){
        sprintf(file_name, "AUCTIONS/%03d/START_%03d.txt", aid, aid);
        fptr = fopen(file_name, "r");
        if(fptr == NULL) return 0;
        fscanf(fptr, "%*s %*s %*s %d %*s", &min_bid);
        fclose(fptr);
    }
    else{
        min_bid = getBiggestBid(aid);
    }
    printf("min_bid: %d\n", min_bid);
    if (bid > min_bid) return 1;
    return 0;
}

int createBid(int aid, char* uid, int bid){ //UID bid_value bid_date bid_time bid_sec_time
    char file_name[45];
    memset(file_name, 0, 45);
    FILE *fptr;
    time_t start_time, current_time, seconds;

    printf("aid: %03d\n", aid);

    sprintf(file_name,"AUCTIONS/%03d/START_%03d.txt", aid, aid);

    printf("file_name: %s\n", file_name);

    fptr = fopen(file_name, "r");
    if(fptr == NULL) return -1;
    fscanf(fptr, "%*s %*s %*s %*d %*d %*d-%*d-%*d %*d:%*d:%*d %ld\n", &start_time);
    fclose(fptr);

    memset(file_name, 0, 45);
    sprintf(file_name, "AUCTIONS/%03d/BIDS/%06d.txt", aid, bid);
    fptr = fopen(file_name, "w");
    if(fptr == NULL) return -1;
    
    time(&current_time);
    struct tm tm = *localtime(&current_time);
    seconds = current_time - start_time;

    fprintf(fptr, "%s %d %04d-%02d-%02d %02d:%02d:%02d %ld\n", uid, bid, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, seconds);
    
    fclose(fptr);

    memset(file_name, 0, 45);
    sprintf(file_name, "USERS/%s/BIDDED/%03d.txt", uid, aid);
    fptr = fopen(file_name, "w");
    if(fptr == NULL) return -1;

    fprintf(fptr, "%d\n", bid);

    fclose(fptr);

    return 1;
}

int writeAuctionData(int aid, char *data, size_t bytesRead){
    char file_path[128];
    char file_name[35];
    FILE *fptr;
    char straid[4];

    printf("aid: %03d\n", aid);
    printf("data: %s\n", data);

    sprintf(straid, "%03d", aid);

    getAssetFileName(straid, file_name);

    sprintf(file_path, "AUCTIONS/%03d/ASSET/%s", aid, file_name);
    fptr = fopen(file_path, "wb");
    if(fptr == NULL) return 0;
    fwrite(data, 1, bytesRead, fptr);
    fclose(fptr);
    return 1;
}


