#ifndef SHARED_H
#define SHARED_H

#include <SDL.h>
#include <SDL_net.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_CLIENTS 8
#define PORT 1234
#define JON_IP "130.229.176.33"

bool NET_serverInitSDL();
void NET_severDestroySDL();




#endif