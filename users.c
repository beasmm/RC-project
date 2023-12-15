#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include "users.h"
#include "constants_udp.h"

int createUserDir(char *uid){
    char uid_dirname[15];
    char hosted_dirname[20];
    char bidded_dirname[20];
    int ret;

    if (strlen(uid) != 6) return 0;

    sprintf(uid_dirname, "USERS/%s", uid);

    ret = mkdir(uid_dirname, 0700);
    if (ret == -1) return 0;

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

int updateLogin(char *uid){
    char login_name[35];
    FILE *fptr;

    if(strlen(uid) != 6) return 0;

    sprintf(login_name, "USERS/%s/%s_login.txt", uid, uid);
    fptr = fopen(login_name, "w");
    if(fptr == NULL) return 0;

    fprintf(fptr, "Logged out\n");
    
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

    fscanf(fptr, "%s", login);

    int ret = strcmp(login, "Logged in\n");

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

//user commands

/* int getUser(User user, char *buf){
    strncpy(user.uid, buf, UID_SIZE);
    user.uid[UID_SIZE] = '\0';

    strncpy(user.password, buf + UID_SIZE + 1, PASSWORD_SIZE);
    user.password[PASSWORD_SIZE] = '\0';
} */
