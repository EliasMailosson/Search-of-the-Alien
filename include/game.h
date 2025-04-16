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
#include "../include/MAP/map.h"


typedef struct menu Menu;
typedef struct {
    SDL_Window *pWin;
    SDL_Renderer *pRend;
    int windowHeight, windowWidth;
    bool windowFullscreen;
    char myUsername[40];
}ClientView; 

typedef struct {
    bool isRunning;
    SDL_Event event;
    bool keys[SDL_NUM_SCANCODES];
    bool isMouseDown, isMouseUp;
    SDL_Point mousePos;

    bool isTextInput;
    char textInput[32];
}ClientControl;

void render(ClientView *pView, Menu *pMenu);
void eventHandler(ClientControl *pControl);
void gameLoop(Client aClient, ClientControl *pControl, ClientView *pView);
void runLobby(Client aClient, Map aMap, ClientControl *pControl, ClientView *pView);
void runMenu(Client aClient, ClientControl *pControl, ClientView *pView, Menu *pMenu);
void toggleFullscreen(ClientView *pView);

#endif
