#define CODE_SIZE 4
#define MAX_CMD_SIZE 11
#define UID_SIZE 7
#define PASSWORD_SIZE 9

#define CASE_LOGIN 0
#define CASE_LOGOUT 1
#define CASE_UNREGISTER 2
#define CASE_MYAUCTIONS 3
#define CASE_MYBIDS 4
#define CASE_SHOW_RECORD 5
#define CASE_LIST 6

#define FNAME 24
#define FSIZE 8

typedef struct{
    char name[15];
    char asset_fname[FNAME];
    int start_value;
    int timeactive;
    size_t size[FSIZE];
    char data[15];
    int aid;
    int higher_value;

} Auction;

typedef struct User{
    char uid[UID_SIZE];
    char password[PASSWORD_SIZE];
    int logged_in;
    Auction auction[1000];
}User;