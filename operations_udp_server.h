#ifndef OPERATIONS_UPD_SERVER_H
#define OPERATIONS_UPD_SERVER_H

int compareStrings(const void *a, const void *b);

// Function declarations
int login(char *buffer);
int logout(char *buffer);
int unregister(char *buffer);
int myauctions(char *buffer);
int mybids(char *buffer);
int list(char *buffer);

#endif // OPERATIONS_UPD_SERVER_H