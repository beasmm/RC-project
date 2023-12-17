#ifndef CONSTANTS_TCP_H
#define CONSTANTS_TCP_H

//Client send operations:
int client_open(char *buffer, Auction auction, int uid, char password[PASSWORD_SIZE]);
int client_close(char *buffer, Auction auction, int uid, char password[PASSWORD_SIZE]);
int client_show_asset(char *buffer, Auction auction);
int client_bid(char *buffer, Auction auction, int uid, char password[PASSWORD_SIZE]);

//client answer operations
int client_open_answer(char *buffer);
int client_close_answer(char *buffer);
int client_show_asset_answer(char *buffer);

#endif /* CONSTANTS_TCP_H */
