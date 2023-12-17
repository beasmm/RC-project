#include "constants_udp.h"
#include <stdio.h>
#include <string.h>



//Client send operations:
int client_login(char *buffer, User *user){

    sscanf(buffer, "login %s %s\n", user->uid, user->password);
    user->uid[UID_SIZE] = '\0';
    user->password[PASSWORD_SIZE] = '\0';

    memset(buffer, 0, 128);

    if (strlen(user->uid) != UID_SIZE) { //register new user
        printf("invalid uid\n");
        return -1;
    }
    if (strlen(user->password) != PASSWORD_SIZE) { //register new user
        printf("invalid password\n");
        return -1;
    }
    sprintf(buffer, "LIN %s %s\n", user->uid, user->password);
    return 0;
}

int client_logout(char *buffer, User *user){
    memset(buffer, 0, 128);
    sprintf(buffer, "LOU %s %s\n", user->uid, user->password);
    return 0;
}

int client_unregister(char *buffer, User *user){
    memset(buffer, 0, 128);
    sprintf(buffer, "UNR %s %s\n", user->uid, user->password);
    return 0;
}

int client_myauctions(char *buffer){
    char id[UID_SIZE + 1];
    sscanf(buffer, "%*s %s", id);

    /* if (strlen(uid) != UID_SIZE) {
        printf("invalid uid\n");
        return -1;
    } */

    id[UID_SIZE+1] = '\0';

    memset(buffer, 0, 128);
    sprintf(buffer, "LMA %s\n", id);
    return 0;
}

int client_mybids(char *buffer){
    char uid[UID_SIZE+1];
    sscanf(buffer, "%*s %s", uid);

    if (strlen(uid) != UID_SIZE) {
        printf("invalid uid\n");
        return -1;
    }

    uid[UID_SIZE] = '\0';

    memset(buffer, 0, 128);
    sprintf(buffer, "LMB %s\n", uid);
    return 0;
}

int client_show_record(char *buffer){
    char aid[4];
    memset(aid, 0, 4);
    sscanf(buffer, "show_record %s", aid);
    memset(buffer, 0, 128);
    sprintf(buffer, "SRC %s\n", aid);
    return 0;
}

//print auctions
void print_auctions(char str[]){
    char aid[4];
    int i = 7, j = 0;

    //while(str[i] != '\n' && str[i] != '\0') {
    while(str[i] != '\0' && str[i] != '\n') {
        while (j < 3) {
            aid[j++] = str[i++];
            if (str[i] < '0' || str[i] > '9') break;
        }

        if (str[i] != ' ' && (str[i] < '0' || str[i] > '9')) break;

        aid[j] = '\0';
        i++;

        printf("id: %s --> ", aid);

        if (str[i] == '0') printf("state: inactive\n");
        else if (str[i] == '1') printf("state: active\n");
        j = 0;
        i += 2;
    }
    printf("\n");

    return;
}


//Client answer operations:
int client_login_answer(char *buffer){
    if (strcmp("OK\n", buffer + 4) == 0) {
        printf("successful login\n");
        return 1;
    }
    else if (strcmp("NOK\n", buffer + 4) == 0) {
        printf("incorrect login attempt\n");
        return 0;
    }
    else {
        printf("new user registered\n");
        return 1;
    }
    return 0;
}

int client_logout_answer(char *buffer){
    if (strcmp("OK\n", buffer + 4) == 0) {
        printf("successful logout\n");
        return 1;
    }
    else if (strcmp("UNR\n", buffer + 4) == 0) {
        printf("unknow user\n");
        return 0;
    }
    else if (strcmp("NOK\n", buffer + 4) == 0) {
        printf("user not logged in\n");
        return 0;
    }
    return 0;
}

int client_unregister_answer(char *buffer){
    if (strcmp("OK\n", buffer + 4) == 0) {
        printf("successful unregister\n");
        return 1;
    }
    else if (strcmp("UNR\n", buffer + 4) == 0) {
        printf("unknow user\n");
        return 0;
    }
    else {
        printf("user not logged in\n");
        return 0;
    }
    return 0;
}

int client_myauctions_answer(char *buffer){
    if (strcmp("NLG\n", buffer + 4) == 0) {
        printf("user not logged in\n");
        return 0;
    }
    else if (strcmp("NOK\n", buffer + 4) == 0) {
        printf("user has no ongoing auctions\n");
        return 0;
    }
    else {
        printf("ongoing auctions started by user:\n");
        print_auctions(buffer);
        return 0;
    }
}

int client_mybids_answer(char *buffer){
    if (strcmp("NLG\n", buffer + 4) == 0) {
        printf("user not logged in\n");
        return 0;
    }
    else if (strcmp("NOK\n", buffer + 4) == 0) {
        printf("user has no ongoing bids\n");
        return 0;
    }
    else {
        printf("ongoing auctions with bids from user:\n");
        print_auctions(buffer);
        return 0;
    }
}

int client_show_record_answer(char *buffer){//TODO
    printf("show_record buffer: %s\n", buffer);
    return 1;
}

int client_list_answer(char *buffer){
    if (strcmp("NOK\n", buffer + 4) == 0) {
        printf("no auction started yet\n");
        return 0;
    }
    else {
        printf("ongoing auctions:\n");
        print_auctions(buffer);
        return 0;
    }
}

