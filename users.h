#ifndef USERS_H
#define USERS_H


int initUsers();

int createUserDir(char *uid);

int createLogin(char *uid);

int updateLogin(char *uid);

int eraseLogin(char *uid);

int createPass(char *uid, char *pass);

int erasePass(char *uid);

int checkRegistered(char *uid);

int checkLogin(char *uid);

/**
 * @brief Checks if the password is correct
 * @return int = 0 if correct, != 0 if incorrect
 */
int checkPassword(char *uid, char *pass);

#endif // USERS_H
