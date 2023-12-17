#ifndef AUCTION_H
#define AUCTION_H

#include <stdio.h>

int initAuctions();

int checkActive(int aid);

int createAuctionDir(int aid);

int getAssetFileName(char *aid, char asset_fname[]); //returns 0if no asset was found

int checkAssetFile(char *asset_fname);

int auctionExists(int aid);

#endif // AUCTION_H
