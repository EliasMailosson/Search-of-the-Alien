#ifndef MENU_H
#define MENU_H

#include "UI/panel.h"
// #include "UI/button.h"
// #include "UI/label.h"
// #include "UI/checklist.h"

void renderMenu(SDL_Renderer *pRend, Panel aPanel);
Panel initMenu(SDL_Renderer *pRend);

#endif