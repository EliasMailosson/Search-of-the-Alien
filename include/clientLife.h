#ifndef CLIENTLIFE_H
#define CLIENTLIFE_H
#include "game.h"

void startClient(Client *aClient, ClientView *pView, ClientControl *pControl);

void RenderPlayerName(Client aClient, ClientView *pView, int i, SDL_Rect playerRect);
void killClient(Client *aClient, ClientView *pView);


#endif
