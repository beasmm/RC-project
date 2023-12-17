#include <sys/types.h>

#define CODE_SIZE 4
#define MAX_CMD_SIZE 11
#define UID_SIZE 6
#define PASSWORD_SIZE 8

#define CASE_LOGIN 0
#define CASE_LOGOUT 1
#define CASE_UNREGISTER 2
#define CASE_MYAUCTIONS 3
#define CASE_MYBIDS 4
#define CASE_SHOW_RECORD 5
#define CASE_LIST 6

#define FNAME 24
#define FSIZE 8
#define CODE_SIZE 4
#define MAX_CMD_SIZE 11
#define UID_SIZE 6
#define PASSWORD_SIZE 8

typedef struct User{
    char uid[10];
    char password[10];
    int logged_in;
}User;

typedef struct{
    char name[15];
    char asset_fname[FNAME];
    int start_value;
    int timeactive;
    ssize_t size;
    char data[15];
    int aid;
    int higher_value;
}Auction;

enum Command {
    CMD_LOGIN,
    CMD_LOGOUT,
    CMD_UNREGISTER,
    CMD_MYAUCTIONS,
    CMD_MYBIDS,
    CMD_SHOW_RECORD,
    CMD_LIST,
    CMD_OPA,
    CMD_CLS,
    CMD_SAS,
    CMD_BID,
    CMD_EXIT,
    CMD_ERROR,
};
