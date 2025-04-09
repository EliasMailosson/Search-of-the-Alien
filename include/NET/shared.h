#ifndef SHARED_H
#define SHARED_H

#include <SDL.h>
#include <SDL_net.h>
#include <stdio.h>
#include <stdbool.h>

#include "../../include/NET/protocol.h"

#define MAX_CLIENTS 8
#define PORT 1234
#define JON_IP "130.229.157.115"
//130.229.157.115
/** Initializes SDL network 
 * ex use: While true then the network is running
 */
bool NET_serverInitSDL();
/** Destroying SDL network and SDL*/
void NET_serverDestroySDL();

void NET_PlayerListRead(char* selfUsername);
void NET_eventHandler(bool *pIsRunning, bool *pKeys,SDL_Event event);

#endif