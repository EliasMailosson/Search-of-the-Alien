#ifndef SERVER_H
#define SERVER_H
#include "../../include/NET/shared.h"
#include "../../include/NET/protocol.h"
#include "../../include/NET/enemies.h"

#define MAX_COLORS 8

typedef struct Player Player;

typedef struct User User;

struct server;
typedef struct server *Server;

/** Allocating memory for server
 * UDP connection
 */
Server NET_serverCreate();

//server send
void NET_serverSendInt(Server aServer,GameState GS, MessageType msgType,int placeHolder, int index);
void NET_serverSendString(Server aServer,GameState GS, MessageType msgType, const char* str, int index);
void NET_serverSendArray(Server aServer,GameState GS, MessageType msgType, const void* array, Uint32 arraySize, int index);

void NET_serverSendPlayerPacket(Server aServer,GameState GS);

//server respons
void NET_serverClientConnected(Packet aPacket, Server aServer);
void NET_serverClientDisconnect(Server aServer);
void NET_serverChangeGameStateOnClient(Server aServer,Packet aPacket);

void NET_serverUpdatePlayer(Server aServer, Packet aPacket);

void NET_serverUpdateEnemies(Server aServer, Enemies aEnemies);

//server util
int NET_serverFindPlayerID(Server aServer, const char* str);
int NET_serverCompIP(Server aServer);
void NET_serverRemoveUser(Server aServer,int index);
void NET_serverAddUser(Server aServer, User newUser);

/** Freeing memory after use, for server */
void NET_serverDestroy(Server aServer);

int NET_serverAssignColorIndex(Server aServer);

#endif