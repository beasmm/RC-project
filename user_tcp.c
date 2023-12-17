#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "user_tcp.h"

//global variables
Auction auction;

enum Command get_client_cmd(char *buffer){
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