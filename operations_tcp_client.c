#include <stdio.h>
#include <string.h>
#include "constants_client.h"


//Client send operations:
int client_open(char *buffer, Auction auction, User user){
    FILE *fptr;
    sscanf(buffer, "open %s %s %d %d\n",auction.name,auction.asset_fname,&auction.start_value,&auction.timeactive);

    fptr = fopen(auction.asset_fname,"r");
    if(fptr == NULL){
        printf("Error opening file\n");                    //??
        return -1;
    }
    else{
        fseek(fptr, 0, SEEK_END);                         //find size of file
        auction.size = ftell(fptr)+1;
        fseek(fptr, 0, SEEK_SET);                         //find begining of file
        fread(auction.data, auction.size, 1,fptr);
        fclose(fptr);
    }

    memset(buffer, 0, 128);

    sprintf(buffer,"OPA %s %s %s %d %d %s %ln %s\n", user.uid, user.password, auction.name, auction.start_value, auction.timeactive, auction.asset_fname, &auction.size, auction.data);
    return 0;
}

int client_close(char *buffer, Auction auction, User user){
    sscanf(buffer, "close %d\n", &auction.aid);

    memset(buffer, 0, 128);

    sprintf(buffer,"CLS %s %s %d\n",user.uid, user.password, auction.aid);
    return 0;
}

int client_show_asset(char *buffer, Auction auction){
    if(buffer[1]=='h') sscanf(buffer, "show_asset %d\n", &auction.aid);
    else sscanf(buffer, "sa %d\n", &auction.aid);

    memset(buffer, 0, 128);

    sprintf(buffer,"SAS %d\n", auction.aid);
    return 0;
}

int client_bid(char *buffer, Auction auction, User user){
    int bid;
    if(buffer[1]=='i') sscanf(buffer, "bid %d\n", &bid);
    else sscanf(buffer, "b %d\n", &bid);

    memset(buffer, 0, 128);

    sprintf(buffer,"BID %s %s %d %d\n", user.uid, user.password, auction.aid, bid);
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
        printf("Auction %s opened\n", buffer + 7);
        return 1;
    }
    else if(strcmp("ERR\n", buffer + 4)==0){
        printf("An error occurred while opening the auction\n");
        return 0;
    }
}

int client_close_answer(char *buffer){
    if(strcmp("EUA\n", buffer + 4)==0){
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
}

int client_show_asset_answer(char *buffer){
    if(strcmp("NOK\n",buffer + 4)==0){
        printf("Asset does not exist.\n");
        return 0;
    }
    else if(strncmp("OK", buffer + 4, 2)==0){
        printf("Asset: %s\n", buffer + 7);
        return 1;
    }
    else if(strcmp("ERR\n", buffer + 4)==0){
        printf("An error occurred while showing asset\n");
        return 0;
    }
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
}
