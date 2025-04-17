#ifndef CLIENT_H
#define CLIENT_H
#include <stdbool.h>
#include "../../include/NET/shared.h"

typedef struct Player Player;

struct client;
typedef struct client *Client;

/** Connecting to host server via a specefic address and port */
bool NET_clientConnect(Client aClient);
/** Allocation memory for the client */
Client NET_clientCreate();
/** Freeing client memory after use and sett to NULL */
void NET_clientDestroy(Client aClient);

int NET_clientGetPlayerCount(Client aClient);
SDL_Point NET_clientGetPlayerPos(Client aClient, int playerIdx);

int NET_clientGetState(Client aClient);
void NET_clientSendInt(Client aClient,GameState GS, MessageType msgType,int placeHolder);
void NET_clientSendString(Client aClient,GameState GS, MessageType msgType,const char* str);
void NET_clientSendArray(Client aClient,GameState GS, MessageType msgType,const void* array, Uint32 arraySize);
void NET_clientSetSelfName(Client aClient, char* newName);
void NET_getSelfname(Client aClient, char* outputName);
int NET_clientGetPlayerDirection(Client aClient, int playerIdx);
void NET_clientGetSelfname(Client aClient, char* outputName);

void NET_clientReceiver(Client aClient);
int NET_clientFindPlayer(Client aClient, char* str);

void NET_clientUpdateGameState(Client aClient,Packet aPacket);
void NET_clientUpdatePlayerList(Client aClient,Packet aPacket);
//----------
SDL_Point NET_clientGetSelfPos(Client aClient);


#endif