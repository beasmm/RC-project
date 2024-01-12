#ifndef OPERATIONS_SERVER_H
#define OPERATIONS_SERVER_H


int compareStrings(const void *a, const void *b);

// Function declarations
int login(char *buffer);
int logout(char *buffer);
int unregister(char *buffer);
int myauctions(char *buffer);
int show_record(char *buffer);
int mybids(char *buffer);
int list(char *buffer);

int open_server(char *buffer, int aid, char file_name[]);
int close_server(char *buffer);
int show_asset_server(char *buffer, char file_name[]);
int bid_server(char *buffer);

//sending and transfering files
int server_sendFile(int fd, char* filename, int size);
int server_receiveFile(int fd, char* filename, int size);

#endif // OPERATIONS_SERVER_H
