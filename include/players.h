#ifndef PLAYERS_H
#define PLAYERS_H

#include "game.h"

PlayerInputPacket prepareInputArray(ClientControl *pControl);
void renderPlayers(Client aClient, ClientView *pView);

#endif