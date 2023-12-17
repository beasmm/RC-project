#define FNAME 24
#define FSIZE 8
#define CODE_SIZE 4
#define MAX_CMD_SIZE 11
#define UID_SIZE 6
#define PASSWORD_SIZE 8

#define PORT "58011"

typedef struct{
    char name[15];
    char asset_fname[FNAME];
    int start_value;
    int timeactive;
    size_t size[FSIZE];
    char data[15];
    int aid;
    int higher_value;
}Auction;

enum Command{
    CMD_OPA,
    CMD_CLS,
    CMD_SAS,
    CMD_BID,
    CMD_EXIT,
    CMD_ERROR,
};