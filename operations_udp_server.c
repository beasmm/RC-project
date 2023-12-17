#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include "constants_udp.h"
#include "users.h"
#include "auction.h"

int login(char *buffer){
    char uid[UID_SIZE+1];
    char password[PASSWORD_SIZE+1];

    strncpy(uid, buffer + 4, UID_SIZE);
    uid[UID_SIZE] = '\0';

    strncpy(password, buffer + 4 + UID_SIZE + 1, PASSWORD_SIZE);
    password[PASSWORD_SIZE] = '\0';

    memset(buffer, 0, 128);

    if (checkRegistered(uid) == -1) { //register new user
        createUserDir(uid);
        createLogin(uid);
        createPass(uid, password);
        sprintf(buffer, "RLI REG\n");
        return 0;
    }
    else { //user already exists / has existed in the past
        char path[30];
        sprintf(path, "users/%s/login.txt", uid);
        if (access(path, F_OK) != 0){ // user was registered in the past
            createLogin(uid);
            createPass(uid, password);
            sprintf(buffer, "RLI REG\n");
            return 0;
        }
        else if (checkPassword(uid, password) == 0) {
            updateLogin(uid, 0);
            sprintf(buffer, "RLI OK\n");
            return 0;
        }
        else {
            sprintf(buffer, "RLI NOK\n");
            return 0;
        }
    }
    return -1;
}

int logout(char *buffer){
    char uid[UID_SIZE+1];
    char password[PASSWORD_SIZE+1];

    strncpy(uid, buffer + 4, UID_SIZE);
    uid[UID_SIZE] = '\0';

    strncpy(password, buffer + 4 + UID_SIZE + 1, PASSWORD_SIZE);
    password[PASSWORD_SIZE] = '\0';

    memset(buffer, 0, 128);

    if (checkRegistered(uid) == -1) {
        sprintf(buffer, "RLO UNR\n");
        return 0;
    }

    if (checkLogin(uid) == 0) {
        if (checkPassword(uid, password) == 0) {
            updateLogin(uid, 1);
            sprintf(buffer, "RLO OK\n");
            return 0;
        }
        else {
            sprintf(buffer, "RLO NOK\n");
            return 0;
        }
    }
    return -1;
}

int unregister(char *buffer){
    char uid[UID_SIZE+1];
    char password[PASSWORD_SIZE+1];

    strncpy(uid, buffer + 4, UID_SIZE);
    uid[UID_SIZE] = '\0';

    strncpy(password, buffer + 4 + UID_SIZE + 1, PASSWORD_SIZE);
    password[PASSWORD_SIZE] = '\0';

    memset(buffer, 0, 128);

    if (checkRegistered(uid) == -1) {
        sprintf(buffer, "RUR UNR\n");
        return 0;
    }

    if (checkLogin(uid) == 0) {
        if (checkPassword(uid, password) == 0) {
            eraseLogin(uid);
            erasePass(uid);
            sprintf(buffer, "RUR OK\n");
            return 0;
        }
    }
    else {
        sprintf(buffer, "RUR NOK\n");
        return 0;
    }
    return -1;
}


int list(char *buffer){
    memset(buffer, 0, 128);
    
    if (isDirectoryEmpty("AUCTIONS")){
        sprintf(buffer, "RLS NOK\n");
        return 0;
    }
    else {
        sprintf(buffer, "RLS OK");
        int aid = 1;
        while (auctionExists(aid) != -1){
            char aid_str[5];
            memset(aid_str, 0, 5);
            sprintf(aid_str, " %03d", aid);
            strcat(buffer, aid_str);
            if (checkActive(aid)) strcat(buffer, " 1");
            else strcat(buffer, " 0");
            aid++;
        }
        strcat(buffer, "\n");
        return 0;
    }
}
