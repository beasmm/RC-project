#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include "constants.h"


int getAssetData(char *path, char *data, size_t size){
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;  // Failed to open file
    }

    // Read data from the file
    size_t bytesRead = fread(data, sizeof(char), size, file);
    if (ferror(file)) {
        perror("Error reading file");
        fclose(file);
        return 0;  // Failed to read from file
    }

    // Null-terminate the buffer
    data[bytesRead] = '\0';

    // Close the file
    fclose(file);

    return 1;  // Success
}

long int checkAssetSize(char *path){

    struct stat filestat;
    int ret_stat;

    ret_stat = stat(path, &filestat);

    if(ret_stat == -1 || filestat.st_size == 0) return 0;

    return (filestat.st_size);
}

//Client send operations:
int client_open(char *buffer, Auction auction, User user, char file_name[], long int *filesize){
    sscanf(buffer, "open %s %s %d %d\n", auction.name, auction.asset_fname, &auction.start_value, &auction.timeactive);

    char path[128];
    sprintf(path, "ASSETS/%s", auction.asset_fname);
    struct stat st = {0};
    if (stat(path, &st) != 0) {
        printf(".File %s does not exist\n", auction.asset_fname);
        return 0;
    }
    auction.size = checkAssetSize(path);

    strcpy(file_name, path);
    *filesize = auction.size;

    memset(buffer, 0, strlen(buffer));

    sprintf(buffer,"OPA %s %s %s %d %d %s %ld \n", user.uid, user.password, auction.name, auction.start_value, auction.timeactive, auction.asset_fname, auction.size);

    return 0;
}

int client_close(char *buffer, Auction auction, User user){
    sscanf(buffer, "close %d\n", &auction.aid);

    memset(buffer, 0, 128);

    sprintf(buffer,"CLS %s %s %03d\n",user.uid, user.password, auction.aid);
    return 0;
}

int client_show_asset(char *buffer, Auction auction){
    if(buffer[1]=='h') sscanf(buffer, "show_asset %d\n", &auction.aid);
    else sscanf(buffer, "sa %d\n", &auction.aid);

    memset(buffer, 0, MAXLINE);

    sprintf(buffer,"SAS %03d\n", auction.aid);
    return 0;
}

int client_bid(char *buffer, User user){
    if (strlen(user.uid) == 0) {
        printf("Please login first\n");
        return -1;
    }
    int aid, value;
    if(buffer[1]=='i') sscanf(buffer, "bid %d %d\n", &aid, &value);
    else sscanf(buffer, "b %d %d\n", &aid, &value);

    memset(buffer, 0, MAXLINE);

    sprintf(buffer,"BID %s %s %03d %d\n", user.uid, user.password, aid, value);
    return 0;
}

//client answer operations
int client_open_answer(char *buffer){
    if(strcmp("NOK\n", buffer + 4)==0){
        printf("Auction could not be started\n");
        return 0;
    }
    else if(strcmp("NLG\n", buffer + 4)==0){
        printf("User not logged in\n");
        return 0;
    }
    else if(strncmp("OK", buffer + 4, 2)==0){
        int aid = atoi(buffer + 7);
        printf("Auction %d opened\n", aid);
        return 1;
    }
    else if(strcmp("ERR\n", buffer + 4)==0){
        printf("An error occurred while opening the auction\n");
        return 0;
    }
    return 0;
}

int client_close_answer(char *buffer){
    if(strcmp("EAU\n", buffer + 4)==0){
        printf("Auction does not exist\n");
        return 0;
    }
    else if( strcmp("EOW\n", buffer + 4)==0){
        printf("You are not the owner of this auction\n");
    }
    else if(strcmp("END\n", buffer + 4)==0){
        printf("Auction has already finished\n");
        return 0;
    }
    else if(strcmp("OK\n", buffer + 4)==0){
        printf("Auction closed successfully\n");
        return 1;
    }
    else if(strcmp("ERR\n", buffer + 4)==0){
        printf("An error occurred while closing the auction\n");
        return 0;
    }
    return 0;
}

int client_show_asset_answer(char *buffer){
    if(strcmp("NOK\n",buffer + 4)==0){
        printf("Asset does not exist.\n");
        return 0;
    }
    else if(strncmp("OK", buffer + 4, 2)==0){
        printf("Asset: %s", buffer + 7);
        return 1;
    }
    else if(strcmp("ERR\n", buffer + 4)==0){
        printf("An error occurred while showing asset\n");
        return 0;
    }
    return 0;
}

int client_bid_answer(char *buffer){
    if(strcmp("NOK\n", buffer + 4)==0){
        printf("Auction is not active\n");
        return 0;
    }
    else if(strcmp("NLG\n", buffer + 4)==0){
        printf("User not logged in\n");
        return 0;
    }
    else if(strcmp("ACC\n", buffer + 4)==0){
        printf("Bid accepted\n");
        return 1;
    }
    else if(strcmp("REF\n", buffer + 4)==0){
        printf("Larger bid has already been made\n");
        return 0;
    }
    else if(strcmp("ILG\n", buffer + 4)==0){
        printf("User trying to bid in his own auction\n");
        return 0;
    }
    else if(strcmp("ERR\n", buffer + 4)==0){
        printf("An error occurred while showing asset\n");
        return 0;
    }
    return 0;
}


//sending and transfering files
int client_sendFile(int fd, char* filepath, int size){
    FILE *file = fopen(filepath, "rb");
    if (file == NULL) return -1;

    int filefd = fileno(file);
    size_t total_sent = 0;
    size_t n_sent, n_read, done;
    char data[MAXLINE];

    // Read data from the file
    while (total_sent < size) {
        memset(data, 0, MAXLINE);
        n_read = read(filefd, data, sizeof(data) - 1);
        if (n_read == -1) {
            perror("Error reading file");
            fclose(file);
            return -1;
        }
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

int client_receiveFile(int fd, char *filepath, int n){
    FILE *file = fopen(filepath, "wb");
    if (file == NULL) return -1;
    
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