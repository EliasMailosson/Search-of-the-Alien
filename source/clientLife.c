#include "../include/clientLife.h"

void startClient(ClientView *pView,ClientControl *pControl){
  SDL_Init(SDL_INIT_EVERYTHING);
  pView->windowHeight = 600;
  pView->windowWidth = pView->windowHeight*16/9;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
    }
    if (TTF_Init() == -1) {
        printf("TTF_Init error: %s\n", TTF_GetError());
    }
    if (SDLNet_Init() == -1) {
        printf("Net_Init error: %s\n", SDLNet_GetError());
    }

    pView->pWin = SDL_CreateWindow("main",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,pView->windowWidth,pView->windowHeight,0);
    if (!pView->pWin) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDLNet_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    pView->pRend = SDL_CreateRenderer(pView->pWin,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!pView->pRend) {
        SDL_DestroyWindow(pView->pWin);
        SDLNet_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
    pControl->isRunning = true;
}

void killClient(ClientView *pView){
    SDL_DestroyRenderer(pView->pRend);
    pView->pRend = NULL;
    SDL_DestroyWindow(pView->pWin);
    pView->pWin = NULL;
    SDLNet_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}