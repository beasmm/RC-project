#ifndef CONSTANTS_TCP_H
#define CONSTANTS_TCP_H

int checkAssetSize(char *path);

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

#endif /* CONSTANTS_TCP_H */
