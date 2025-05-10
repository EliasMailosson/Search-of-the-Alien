#ifndef GAME_H
#define GAME_H
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_net.h>
#include "../include/UI/panel.h"
#include "../include/NET/client.h"
#include "../include/NET/shared.h"
//#include "../include/MAP/map.h"
#include "UI/button.h"
#include "UI/friend.h"
#include "../include/HUD/hud.h"

#define MAX_PLAYER_CHARACTERS 3
#define MAX_PLAYER_TEXTURES 6
#define MAX_BUTTONS_TERMINAL_HUB 4
#define MAX_BUTTONS_PAUSE_MENU 3
#define MAX_PROJECTILE_TEXTURES 5

typedef struct menu Menu;
typedef struct enemies *Enemies;
typedef struct {
    bool isVisible;
    Button button[MAX_BUTTONS_PAUSE_MENU];
}PauseMenu;
typedef struct {
    int hubWidth;
    int hubHeight;
    bool isVisible;
    SDL_Rect hubRect;
    Button button[MAX_BUTTONS_TERMINAL_HUB];
}TerminalHub;
typedef struct {
    SDL_Window *pWin;
    SDL_Renderer *pRend;
    int windowHeight, windowWidth;
    bool windowFullscreen;
    char myUsername[MAX_USERNAME_LEN];

        SDL_Cursor *crosshair;
        SDL_Texture *playerTexture[MAX_PLAYER_TEXTURES];
        int playerRenderSize;
    
    SDL_Texture *vignetteTexture;
    SDL_Texture *enemyTexture;
    TTF_Font *fonts;
    Hud aHud;
    SDL_Point PlayerPos[MAX_CLIENTS];
    SDL_Texture *projectileTexture[MAX_PROJECTILE_TEXTURES];
    SDL_Texture *shadowTexture;
}ClientView; 

typedef struct {
    bool isRunning;
    SDL_Event event;
    bool keys[SDL_NUM_SCANCODES];
    bool isMouseDown, isMouseUp;
    SDL_Point mousePos;

    bool isTextInput;
    char textInput[32];

    int selectedCharacter;
}ClientControl;

void eventHandler(ClientControl *pControl);
void gameLoop(Client aClient, ClientControl *pControl, ClientView *pView);
void runLobby(Client aClient, Map aMap, ClientControl *pControl, ClientView *pView, TerminalHub *pTerminalHub, PauseMenu *pPauseMenu);
void runMenu(Client aClient, ClientControl *pControl, ClientView *pView, Menu *pMenu, Map aMap);
void runPlanet(Client aClient, ClientControl *pControl, ClientView *pView, Map aMap, PauseMenu *pPauseMenu, Menu *pMenu);

void toggleFullscreen(ClientView *pView);

#endif
