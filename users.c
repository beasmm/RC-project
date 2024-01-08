#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>

#include "users.h"
#include "constants.h"


int initUsers(){
    struct stat st = {0};
    int ret;

    if (stat("USERS", &st) == -1) {
        ret = mkdir("USERS", 0777);
        if (ret == -1) {
            printf("failed to create USERS directory\n");
            return 0;
        }
    }
    return 0;
}


int createUserDir(char *uid){
    char uid_dirname[15];
    char hosted_dirname[20];
    char bidded_dirname[20];
    int ret;

    if (strlen(uid) != 6) return 0;

    sprintf(uid_dirname, "USERS/%s", uid);

    ret = mkdir(uid_dirname, 0700);
    if (ret == -1) {
        printf("mkdir failed\n");
        return 0;
    }

    sprintf(hosted_dirname, "USERS/%s/HOSTED", uid);

    ret=mkdir(hosted_dirname, 0700);
    if (ret == -1) {
        rmdir(uid_dirname);
        return 0;
    }

    sprintf(bidded_dirname, "USERS/%s/BIDDED", uid);

    ret=mkdir(bidded_dirname, 0700);
    if (ret == -1) {
        rmdir(uid_dirname);
        rmdir(hosted_dirname);
        return 0;
    }
    return 1;
}

int createLogin(char *uid){
    char login_name[35];
    FILE *fptr;

    if(strlen(uid) != 6) return 0;

    sprintf(login_name, "USERS/%s/%s_login.txt", uid, uid);
    fptr = fopen(login_name, "w");
    if(fptr == NULL) return 0;

    fprintf(fptr, "Logged in\n");
    
    fclose(fptr);

    return 1;
}

int updateLogin(char *uid, int logout){
    char login_name[35];
    FILE *fptr;

    if(strlen(uid) != 6) return 0;

    sprintf(login_name, "USERS/%s/%s_login.txt", uid, uid);
    fptr = fopen(login_name, "w");
    if(fptr == NULL) return 0;

    if (logout) fprintf(fptr, "Logged out\n");
    else fprintf(fptr, "Logged in\n");
    
    fclose(fptr);

    return 1;
}

int eraseLogin(char *uid){
    char login_name[35];

    if(strlen(uid) != 6) return 0;

    sprintf(login_name, "USERS/%s/%s_login.txt", uid, uid);
    unlink(login_name);
    return 1;
}

int createPass(char *uid, char *pass){
    char pass_name[35];
    FILE *fptr;

    if(strlen(uid) != 6) return 0;

    if(strlen(pass) != 8) return 0;

    sprintf(pass_name, "USERS/%s/%s_pass.txt", uid, uid);
    fptr = fopen(pass_name, "w");
    if(fptr == NULL) return 0;

    fprintf(fptr, "%s\n", pass);

    fclose(fptr);

    return 1;
}

int erasePass(char *uid){
    char pass_name[35];

    if(strlen(uid) != 6) return 0;

    sprintf(pass_name, "USERS/%s/%s_pass.txt", uid, uid);
    unlink(pass_name);
    return 1;
}


int checkRegistered(char *uid){
    char path[35];
    struct stat st = {0};

    sprintf(path, "USERS/%s", uid);

    return stat(path, &st);
}


int checkLogin(char *uid){
    char path[35];
    char login[11];
    FILE *fptr;

    sprintf(path, "USERS/%s/%s_login.txt", uid, uid);

    fptr = fopen(path, "r");
    if(fptr == NULL) return -1;

    fscanf(fptr, " %[^\n]", login);

    int ret = strncmp(login, "Logged in", 10);

    fclose(fptr);
    return ret;
}


int checkPassword(char *uid, char *pass){
    char path[35];
    char password[9];
    FILE *fptr;
    

    sprintf(path, "USERS/%s/%s_pass.txt", uid, uid);

    fptr = fopen(path, "r");
    if(fptr == NULL) return -1;

    fscanf(fptr, "%s", password);

    int ret = strcmp(pass, password);

    fclose(fptr);
    return ret;
}

int addToHosted(int aid, char uid[6]){
    char file_path[128];
    FILE *fp;

    sprintf(file_path, "USERS/%s/HOSTED/%03d.txt", uid, aid);
    fp = fopen(file_path, "w");
    if(fp == NULL) return 0;
    
    fclose(fp);

    return 1;
}

int eraseHosted(char *uid, char *aid){
    char path[35];

    sprintf(path, "USERS/%s/HOSTED/%s.txt", uid, aid);

    if (unlink(path) == -1){
        if (errno == ENOENT) return 0;
        else {
            perror("Error deleting file");
            return -1;
        }
    }
    return 1;
}

int eraseBidded(char *uid, char *aid){
    char path[35];

    sprintf(path, "USERS/%s/BIDDED/%s.txt", uid, aid);

    if (unlink(path) == -1){
        if (errno == ENOENT) return 0;
        else {
            perror("Error deleting file");
            return -1;
        }
    }
    return 0;
}

int isDirectoryEmpty(const char *path){
    DIR *dir = opendir(path);
    if (dir == NULL){
        perror("Error opening directory");
        return -1;
    }

    struct dirent *entry;
    int ret = 1;

    while ((entry = readdir(dir)) != NULL){
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            ret = 0;
            break;
        }
    }

    closedir(dir);
    return ret;
}


int emptyDir(char *uid, char *dir_name){
    char path[25];
    sprintf(path, "USERS/%s/%s", uid, dir_name);

    int case_hosted = strcmp(dir_name, "HOSTED");

    DIR *dir = opendir(path);
    if (dir == NULL){
        perror("Error opening directory");
        return -1;
    }

    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        if (case_hosted == 0) eraseHosted(uid, entry->d_name);
        else eraseBidded(uid, entry->d_name);
    }

    return 0;
}

int emptyUsersDir(char *uid){
    char path[15];
    sprintf(path, "USERS/%s", uid);

    eraseLogin(uid);
    erasePass(uid);

    emptyDir(uid, "HOSTED");
    emptyDir(uid, "BIDDED");

    return 0;
}

int closeUsers(){
    const char *path = "USERS";
    
    int is_empty = isDirectoryEmpty(path);

    if (is_empty == -1) return -1;

    if (is_empty){
        if(rmdir(path) != 0) {
            perror("Error deleting USERS directory");
            return -1;
        }
    }

    else {
        DIR *dir = opendir(path);
        if (dir == NULL){
            perror("Error opening directory");
            return -1;
        }

        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL){
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                char uid[7];
                strncpy(uid, entry->d_name, 6);
                uid[6] = '\0';
                emptyUsersDir(uid);
            }
        }

        closedir(dir);

        if(rmdir(path) != 0) {
            perror("Error deleting USERS directory");
            return -1;
        }

    }
    
    return 0;
}

int getListOfFiles(char path[], char *files[]){
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return -1;
    }
    strcat(path, "/");
    
    struct dirent *ent;
    int count = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        files[count] = strdup(ent->d_name);
        count++;
    }
    return count;
}

int getHost(int aid){
    char path[30] = "USERS";
    char *users[999];

    int n_dir = getListOfFiles(path, users);

    for(int i = 0; i < n_dir; i++){
        printf("name: %s\n", users[i]);
    }

    for (int i = 0; i < n_dir; i++){
        sprintf(path, "USERS/%s/HOSTED/%03d.txt", users[i], aid);
        if (access(path, F_OK) == 0) return atoi(users[i]);
    }
    return -1;
}


