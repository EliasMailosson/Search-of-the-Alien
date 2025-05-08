#ifndef PLAYERS_H
#define PLAYERS_H

#include "game.h"

PlayerInputPacket prepareInputArray(ClientControl *pControl, int windowWidth, int windowHeight) ;
void renderPlayers(Client aClient, ClientView *pView);
void renderEnemy(Client aClient, ClientView *pView);
void renderProjectiles(Client aClient, ClientView *pView);
//bool canPlaySound(Uint32 *lastPlayedTime, Uint32 timeMs);

#endif