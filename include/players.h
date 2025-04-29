#ifndef PLAYERS_H
#define PLAYERS_H

#include "game.h"

PlayerInputPacket prepareInputArray(ClientControl *pControl, int windowWidth, int windowHeight) ;
void renderPlayers(Client aClient, ClientView *pView);
void renderProjectiles(Client aClient, Proj *projList, ClientView *pView);

#endif