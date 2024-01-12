#ifndef OPERATIONS_CLIENT_H
#define OPERATIONS_CLIENT_H

#include "constants.h"

int checkAssetSize(char *path);

// Client send operations:
int client_login(char *buffer, User *user);
int client_logout(char *buffer, User *user);
int client_unregister(char *buffer, User *user);
int client_myauctions(char *buffer, User *user);
int client_mybids(char *buffer, User *user);
int client_show_record(char *buffer);

// Print auctions
void print_auctions(char str[]);

// Client answer operations:
int client_login_answer(char *buffer);
int client_logout_answer(char *buffer);
int client_unregister_answer(char *buffer);
int client_myauctions_answer(char *buffer);
int client_mybids_answer(char *buffer);
int client_show_record_answer(char *buffer);
int client_list_answer(char *buffer);

//Client send operations:
int client_open(char *buffer, Auction auction, User user, char img_name[], long int *bytes_img);
int client_close(char *buffer, Auction auction, User user);
int client_show_asset(char *buffer, Auction auction);
int client_bid(char *buffer, User user);

//client answer operations
int client_open_answer(char *buffer);
int client_close_answer(char *buffer);
int client_show_asset_answer(char *buffer);
int client_bid_answer(char *buffer);

//sending and transfering files
int client_sendFile(int fd, char* filename, int size);
int client_receiveFile(int fd, char* filename, int size);

#endif /* CONSTANTS_TCP_H */
