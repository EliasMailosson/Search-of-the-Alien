#ifndef MENU_H
#define MENU_H

#include "UI/panel.h"
#include "game.h"

#define PANEL_COUNT 3
#define FONT_COUNT 2

enum MenuPanels {PANEL_START, PANEL_SOCIAL, PANEL_OPTIONS};

typedef struct menu {
    Panel panels[PANEL_COUNT];
    int currentPanel;

    TTF_Font *fonts[FONT_COUNT];
} Menu;

void renderMenu(SDL_Renderer *pRend, Menu *pMenu);
Menu initMenu(SDL_Renderer *pRend, ClientView *pView);
void updateMenu(Menu *pMenu, ClientControl *pControl);
void destroyMenu(Menu *pMenu);

#endif