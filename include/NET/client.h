#ifndef CLIENT_H
#define CLIENT_H
#include <stdbool.h>

struct client;
typedef struct client *Client;

/** Initializes the whole packege and what to send */
void NET_clientSend(Client aClient);
/** Connecting to host server via a specefic address and port */
bool NET_clientConnect(Client aClient);
/** Allocation memory for the client */
Client NET_clientCreate();
/** Freeing client memory after use and sett to NULL */
void NET_clientDestroy(Client aClient);




#endif