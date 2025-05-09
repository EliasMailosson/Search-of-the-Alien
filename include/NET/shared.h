#ifndef SHARED_H
#define SHARED_H

#include <SDL.h>
#include <SDL_net.h>
#include <stdio.h>
#include <stdbool.h>

#include "../../include/NET/protocol.h"
#include "server.h"

#define PROJECTILEWIDTH 20
#define PROJECTILEHEIGHT 20

#define MAX_CLIENTS 8
#define PORT 1234
#define JON_IP "130.229.146.223"

/** Initializes SDL network 
 * ex use: While true then the network is running
 */
bool NET_serverInitSDL();
/** Destroying SDL network and SDL*/
void NET_serverDestroySDL();  

void NET_PlayerListRemovePlayer(PlayerPacket **list, int index, int *listCount);
void NET_PlayerListAddPlayer(PlayerPacket **list,PlayerPacket newPlayer,int *listCount);

void NET_PlayerListUpdate(Packet aPacket, PlayerPacket* list, int *count);
void NET_PlayerListPrintf(PlayerPacket* list, int count);
void NET_eventHandler(bool *pIsRunning, bool *pKeys,SDL_Event event);

#endif