#ifndef CLIENT_H
#define CLIENT_H

struct client;
typedef struct client *Client;

Client NET_clientCreate();
void NET_clientDestroy(Client aClient);
bool NET_clientConnect(Client aClient);
void NET_clientSend(Client aClient);



#endif