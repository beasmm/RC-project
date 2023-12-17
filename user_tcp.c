#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "constants_tcp.h"
#include "constants_udp.h"
#include "operations_tcp_client.h"

//global variables
Auction auction;

enum Command get_client_cmd_tcp(char *buffer){
    switch(buffer[0]){
        case 'o': // open
            return CMD_OPA;
        case 'c': //close
            return CMD_CLS;
        case 's': //show asset
            return CMD_SAS;
        case 'b': //bid
            return CMD_BID;
        case 'e':
            if(strcmp("exit", buffer) == 0)
                return CMD_EXIT;
            return CMD_ERROR;
        default:
            return CMD_ERROR;
    }
}

int execute_cmd_client_tcp(char *buffer, Auction auction, User *user){
    switch(get_client_cmd_tcp(buffer)){
        case CMD_OPA:
            return client_open(buffer,auction, user);
        case CMD_CLS:
            return client_close(buffer,auction, user);
        case CMD_SAS:
            return client_show_asset(buffer, auction);
        case CMD_BID:
            return client_bid(buffer, auction, user);
        case CMD_EXIT:
            return 0;
        default:
            printf("Invalid command\n");
            return -1;
    }
}

enum Command get_answer_cmd_tcp(char *buffer){
    switch (buffer[1]){
    case 'O':
        return CMD_OPA;
    case 'C':
        return CMD_CLS;
    case 'S':
        return CMD_SAS;
    case 'B':
        return CMD_BID;    
    default:
        return CMD_ERROR;
    }
}

int get_answer_tcp(char *buffer){
    switch(get_answer_cmd_tcp(buffer)){
        case CMD_OPA:
            return client_open_answer(buffer);
        case CMD_CLS:
            return client_close_answer(buffer);
        case CMD_SAS:
            return client_show_asset_answer(buffer);
        case CMD_BID:
            return client_bid_answer(buffer);
        default:
            printf("Invalid command\n");
            break;
    }
}