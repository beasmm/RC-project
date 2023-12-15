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

typedef struct User{
    char uid[UID_SIZE];
    char password[PASSWORD_SIZE];
    int logged_in;
}User;


enum Command {
    CMD_LOGIN,
    CMD_LOGOUT,
    CMD_UNREGISTER,
    CMD_MYAUCTIONS,
    CMD_MYBIDS,
    CMD_SHOW_RECORD,
    CMD_LIST,
    CMD_EXIT,
    CMD_ERROR,
};

