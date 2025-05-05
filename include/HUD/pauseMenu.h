#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include "../UI/button.h"
#include "../game.h"

void initPauseMenu(ClientView *pView, PauseMenu *pPm);
void renderPauseMenu(ClientView *pView, PauseMenu *pPm);
void updatePauseMenu(PauseMenu *pPauseMenu, Client aClient, ClientControl *pControl, int *pDelay, Menu *pMenu);
void destroyPauseMenu(PauseMenu *pPauseMenu);
void refreshPauseMenu(ClientView *pView, PauseMenu *pPm);

#endif