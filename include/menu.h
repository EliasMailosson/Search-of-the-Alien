#ifndef MENU_H
#define MENU_H

#include "UI/panel.h"
#include "../include/game.h"

void renderMenu(SDL_Renderer *pRend, Panel aPanel);
Panel initMenu(SDL_Renderer *pRend, ClientView *pView);
void updateMenu(Panel aPanel, ClientControl *pControl);

#endif