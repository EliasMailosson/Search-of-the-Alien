#ifndef GAME_H
#define GAME_H
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_net.h>
#include "../include/UI/panel.h"

typedef struct menu Menu;
typedef struct {
    SDL_Window *pWin;
    SDL_Renderer *pRend;
    int windowHeight, windowWidth;
}ClientView; 

typedef struct {
    bool isRunning;
    SDL_Event event;
    bool keys[SDL_NUM_SCANCODES];
    bool isMouseDown, isMouseUp;
}ClientControl;

void render(ClientView *pView, Menu *pMenu);
void eventHandler(ClientControl *pControl);
void gameLoop(ClientControl *pControl, ClientView *pView);

#endif
