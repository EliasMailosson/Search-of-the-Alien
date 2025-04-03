#ifndef SHARED_H
#define SHARED_H

#include <SDL.h>
#include <SDL_net.h>
#include <stdio.h>
#include <stdbool.h>

#include "../../include/NET/protocol.h"

#define MAX_CLIENTS 8
#define PORT 1234
#define JON_IP "130.229.190.87"

/** Initializes SDL network 
 * ex use: While true then the network is running
 */
bool NET_serverInitSDL();
/** Destroying SDL network and SDL*/
void NET_severDestroySDL();

typedef struct{
    int ID;
    SDL_Point pos;
}PlayerList;

void NET_PlayerListRemovPlayer(PlayerList *list,int index);
void NET_PlayerListAddPlayer(PlayerList *list,PlayerList new_player);

#endif