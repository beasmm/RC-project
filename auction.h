#ifndef AUCTION_H
#define AUCTION_H

#include <stdio.h>

int initAuctions();

int checkActive(int aid);

int createAuctionDir(int aid);

int getAssetFileName(char *aid, char asset_fname[]); //returns 0if no asset was found

int checkAssetFile(char *asset_fname);

int getAsset(char *asset_fname, char *asset_content);

int auctionExists(int aid);

int auctionIsOwnedByUser(int aid, char *uid); //returns 1 if auction is owned by user, 0 otherwise

#endif // AUCTION_H
