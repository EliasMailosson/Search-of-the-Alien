#ifndef SERVER_H
#define SERVER_H
#include "../../include/NET/shared.h"
#include "../../include/NET/protocol.h"

struct server;
typedef struct server *Server;

/** Allocating memory for server
 * UDP connection
 */
Server NET_serverCreate();

/** Freeing memory after use, for server */
void NET_serverDestroy(Server aServer);

void NET_serverReceivePlayerList(Packet aPkg, PlayerList* list, int *count);
void NET_PlayerListPrintf(PlayerList* list, int count);


#endif