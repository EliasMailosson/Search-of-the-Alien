#ifndef GAME_H
#define GAME_H
#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "../include/UI/panel.h"

typedef struct {
    SDL_Window *pWin;
    SDL_Renderer *pRend;
    int windowHeight, windowWidth;
}ClientView; 

typedef struct {
    bool isRunning;
    SDL_Event event;
    bool keys[SDL_NUM_SCANCODES];
    bool isMouseDown;
}ClientControl;

void render(ClientView *pView, Panel aPanel);
void eventHandler(ClientControl *pControl);
void gameLoop(ClientControl *control, ClientView *view);

#endif
