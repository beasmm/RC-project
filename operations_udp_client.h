#ifndef CONSTANTS_UDP_H
#define CONSTANTS_UDP_H


// Client send operations:
int client_login(char *buffer, User *user);
int client_logout(char *buffer, User *user);
int client_unregister(char *buffer, User *user);
int client_myauctions(char *buffer);
int client_mybids(char *buffer);
int client_show_record(char *buffer);

// Print auctions
void print_auctions(char str[]);

// Client answer operations:
int client_login_answer(char *buffer);
int client_logout_answer(char *buffer);
int client_unregister_answer(char *buffer);
int client_myauctions_answer(char *buffer);
int client_mybids_answer(char *buffer);
int client_show_record_answer(char *buffer);
int client_list_answer(char *buffer);

#endif /* CONSTANTS_UDP_H */
