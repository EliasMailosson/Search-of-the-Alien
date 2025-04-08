#ifndef MENU_H
#define MENU_H

#include "UI/panel.h"
#include "game.h"

#define PANEL_COUNT 4
#define FONT_COUNT 2

#define OFFSET 80
#define BIGBUTTONWIDTH 300
#define BIGBUTTONHEIGHT 60
#define SMALLBUTTONWIDTH 200
#define SMALLBUTTONHEIGHT 40

enum MenuPanels {PANEL_START, PANEL_GAMESELECTION, PANEL_SOCIAL, PANEL_OPTIONS};
enum MenuEvents {PANEL_IDLE, BUTTON_CLICKED, CHECKLIST_UPDATED, PANEL_SWITCH};

typedef struct menuEvent {
    int eventType;
    char key[256];

    int newPanel;

    bool isTextInput;
    char textInput[32];
} MenuEvent;

typedef struct menu {
    Panel panels[PANEL_COUNT];
    int currentPanel;
    bool isGameStarted;

    TTF_Font *fonts[FONT_COUNT];
} Menu;

void renderMenu(SDL_Renderer *pRend, Menu *pMenu);
void refreshMenu(SDL_Renderer *pRend, Menu *pMenu, ClientView *pView);
Menu initMenu(SDL_Renderer *pRend, ClientView *pView);
void updateMenu(Menu *pMenu, ClientControl *pControl);
void destroyMenu(Menu *pMenu);

#endif