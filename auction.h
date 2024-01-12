#ifndef AUCTION_H
#define AUCTION_H

#include "constants.h"

int getAuctionID();

int printdate(Date date);

int printtime(Time time);

int initAuctions();

int createENDFile(int aid);

int checkActive(int aid);

int checkExpiry(int aid);

int createAuctionDir(int aid);

int getDetailsFromStartFile(int aid, Auction *auction);

int createAssetFile(char *asset_fname);

int getAssetFileName(int aid, char asset_fname[]); //returns 0 if no asset was found

int checkAssetFile(char *asset_fname);

long int getAssetSize(char *path);

int getAsset(char *asset_fname, char *asset_content);

int auctionExists(int aid);

int auctionIsOwnedByUser(int aid, char *uid); //returns 1 if auction is owned by user, 0 otherwise

int getBiggestBid(int aid); //returns biggest bid

int checkBidAmmount(int aid, int bid); //returns 1 if bid is allowed, 0 if it will be refused

int createBid(int aid, char* uid, int bid);

int getDetailsFromBIDSFile(int aid, int bid_amount, char *buffer); //returns 0 if no bids were found

int createStartFile(int aid, char uid[], Auction *auction);

int getDetailsFromENDFile(int aid, char *buffer);

int writeAuctionData(int aid, char* buffer, size_t bytesRead);

#endif // AUCTION_H
