#include <sys/types.h>

#define CODE_SIZE 4
#define MAX_CMD_SIZE 11
#define UID_SIZE 6
#define PASSWORD_SIZE 8
#define MAXLINE 1024

//AUCTION
#define NAME_SIZE 15
#define DATE_SIZE 10
#define TIME_SIZE 8

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

typedef struct Date{
    int day;
    int month;
    int year;
}Date;

typedef struct Time{
    int hour;
    int minute;
    int second;
}Time;

typedef struct User{
    char uid[10];
    char password[10];
    int logged_in;
}User;

typedef struct Auction{
    char host_uid[UID_SIZE+1];
    char name[15];
    char asset_fname[FNAME];
    int start_value;
    int timeactive;
    long int size;
    char *data;
    int aid;
    int higher_value;
    Date start_date;
    Time start_time;
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
