#include <stdio.h>
#include <string.h>

#include "constants.h"


int print_bids_sr(char *buffer){
    char uid[UID_SIZE+1];
    char value[15];
    char date[DATE_SIZE+1];
    char time[TIME_SIZE+1];
    int secs = 0; 

    sscanf(buffer, "B %s %s %s %s %i ", uid, value, date, time, &secs);

    printf("  >  Bidder %s bid %s on %s at %s, %i seconds after the auction started\n", uid, value, date, time, secs);

    return 0;
}

//Client send operations:
int client_login(char *buffer, User *user){

    sscanf(buffer, "login %s %s\n", user->uid, user->password);
    user->uid[UID_SIZE] = '\0';
    user->password[PASSWORD_SIZE] = '\0';

    memset(buffer, 0, 128);

    if (strlen(user->uid) != UID_SIZE) { //register new user
        printf("invalid uid\n");
        return 1;
    }
    if (strlen(user->password) != PASSWORD_SIZE) { //register new user
        printf("invalid password\n");
        return 1;
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

int client_myauctions(char *buffer, User *user){
    char uid[UID_SIZE + 1];
    strcpy(uid, user->uid);

    if (strlen(uid) != UID_SIZE) {
        printf("invalid uid\n");
        return 1;
    }

    uid[UID_SIZE] = '\0';

    memset(buffer, 0, 128);
    sprintf(buffer, "LMA %s\n", uid);
    return 0;
}

int client_mybids(char *buffer, User *user){
    char uid[UID_SIZE+1];
    strcpy(uid, user->uid);

    if (strlen(uid) != UID_SIZE) {
        printf("invalid uid\n");
        return 1;
    }

    uid[UID_SIZE] = '\0';

    memset(buffer, 0, 128);
    sprintf(buffer, "LMB %s\n", uid);
    return 0;
}

int client_show_record(char *buffer){
    int aid;
    sscanf(buffer, "%*s %d", &aid);

    if (aid < 1 || aid > 999) {
        return 1;
    }

    memset(buffer, 0, 128);
    sprintf(buffer, "SRC %03d\n", aid);
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
    if (strcmp("NOK\n", buffer + 4) == 0) {
        printf("auction does not exist\n");
        return 0;
    }
    char host_uid[UID_SIZE+1];
    char auction_name[NAME_SIZE+1];
    char asset_fname[NAME_SIZE+1];
    int start_value; //verif
    char date[DATE_SIZE+1];
    char time[TIME_SIZE+1];
    int timeactive;
    char bids[256];

    sscanf(buffer, "RRC OK %s %s %s %d %s %s %d %[^\n]", 
                    host_uid, auction_name, asset_fname, &start_value, date, time, &timeactive, bids);

    printf("Auction \"%s\" hosted by %s. Asset file name: \"%s\", starting value %d. Started on %s at %s and will last %d seconds.\n", 
            auction_name, host_uid, asset_fname, start_value, date, time, timeactive);

    strcat(bids, "\n");

    int n_bids = 0;
    char aux[70];
    memset(aux, 0, 70);
    int i, j = 0;
    for (i = 0; bids[i] != '\n'; i++) {
        if (bids[i] == 'B') {
            n_bids++;
            if (n_bids == 1) {
                printf("Most recent bids:\n");
            }
            else {
                print_bids_sr(aux);
                memset(aux, 0, 70);
                j = 0;
            }
        }
        else if (bids[i] == 'E') {
            if (n_bids == 0) {
                printf("No bids yet.\n");
            }
            else {
                print_bids_sr(aux);
                memset(aux, 0, 70);
                j = 0;
            }
            break;
        }
        aux[j++] = bids[i];
    }

    if (bids[i] == 'E') {
        j = 0;
        for (i++; bids[i] != '\n'; i++) {
            aux[j++] = bids[i];
        }
        sscanf(aux, "%s %s %d", date, time, &timeactive);
        printf("Auction ended on %s at %s, after %d seconds.\n", date, time, timeactive);
    }

    else printf("Auction ongoing\n");

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

