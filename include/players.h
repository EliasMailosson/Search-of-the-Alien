#ifndef PLAYERS_H
#define PLAYERS_H

#include "game.h"

PlayerInputPacket prepareInputArray(ClientControl *pControl, int windowWidth, int windowHeight) ;
void renderProjectiles(Client aClient, ClientView *pView);
void renderEntities(Client aClient, ClientView *pView);
void RenderPlayerName(ClientView *pView, Player player, SDL_Rect playerRect);

#endif