#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>


#include "constants.h"
#include "users.h"
#include "auction.h"


int compareStrings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

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
        sprintf(path, "USERS/%s/%s_login.txt", uid, uid);
        if (access(path, F_OK) != 0){ // user was registered in the past
            printf("path: %s\n", path);
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

int myauctions(char *buffer){
    char uid[UID_SIZE+1];
    strncpy(uid, buffer + 4, UID_SIZE);
    uid[UID_SIZE] = '\0';

    memset(buffer, 0, 128);

    if (checkLogin(uid) != 0){
        sprintf(buffer, "RMA NLG\n");
        return 0;
    }

    char path[30];
    sprintf(path, "USERS/%s/HOSTED", uid);
    
    if(isDirectoryEmpty(path)){ //no auctions hosted by uid
        sprintf(buffer, "RMA NOK\n");
        return 0;
    }

    sprintf(buffer, "RMA OK");
    char *aids[999];
    int n_aids = getListOfFiles(path, aids);
    char aux[4];
    memset(aux, 0, 4);

    qsort(aids, n_aids, sizeof(aids[0]), compareStrings);

    for (int i = 0; i < n_aids; i++){
        strcat(buffer, " ");
        strncpy(aux, aids[i], 3);
        strcat(buffer, aux);
        int id = atoi(aux);
        if (checkActive(id)) strcat(buffer, " 1");
        else strcat(buffer, " 0");
    }
    strcat(buffer, "\n");

    return 0;
}

int mybids(char *buffer){
    char uid[UID_SIZE+1];
    strncpy(uid, buffer + 4, UID_SIZE);
    uid[UID_SIZE] = '\0';

    memset(buffer, 0, 128);

    if (checkLogin(uid) != 0){
        sprintf(buffer, "RMB NLG\n");
        return 0;
    }

    char path[30];
    sprintf(path, "USERS/%s/BIDDED", uid);
    
    if(isDirectoryEmpty(path)){ //USER HAS NO BIDS
        sprintf(buffer, "RMB NOK\n");
        return 0;
    }

    sprintf(buffer, "RMB OK");
    char *aids[999];
    int n_aids = getListOfFiles(path, aids);
    char aux[4];
    memset(aux, 0, 4);

    qsort(aids, n_aids, sizeof(aids[0]), compareStrings);

    for (int i = 0; i < n_aids; i++){
        strcat(buffer, " ");
        strncpy(aux, aids[i], 3);
        strcat(buffer, aux);
        int id = atoi(aux);
        if (checkActive(id)) strcat(buffer, " 1");
        else strcat(buffer, " 0");
    }
    strcat(buffer, "\n");

    return 0;
}

int show_record(char *buffer){
    char straid[4];
    strncpy(straid, buffer + 4, 3);
    int aid = atoi(straid);

    if (auctionExists(aid) == -1){
        sprintf(buffer, "RRC NOK\n");
        return 0;
    }
    sprintf(buffer, "RRC OK %06d", getHost(aid));

    char asset_filename[128];
    memset(asset_filename, 0, 128);
    getAssetFileName(straid, asset_filename);

    strcat(buffer, " ");
    strcat(buffer, asset_filename);

    return 0;
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
