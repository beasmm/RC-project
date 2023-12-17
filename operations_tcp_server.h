#ifndef AUCTION_SYSTEM_HEADER_H
#define AUCTION_SYSTEM_HEADER_H

extern char state[10][3];
extern int aid;

int open_server(char *buffer);

int close_server(char *buffer);

int show_asset_sever(char *buffer);

int bid_server(char *buffer);

#endif // AUCTION_SYSTEM_HEADER_H
