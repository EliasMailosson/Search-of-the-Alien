#include "game.h"

void startClient(ClientView *pView, ClientControl *pControl);
void killClient(ClientView *pView);

int main(int argc, char **argv ){
    (void)argc; (void)argv;

    ClientView view = {0};
    ClientControl control = {0};

    startClient(&view, &control);
    gameLoop(&control, &view);
    killClient(&view);

    return 0;
}


void killClient(ClientView *pView){
    SDL_DestroyRenderer(pView->pRend);
    pView->pRend = NULL;
    SDL_DestroyWindow(pView->pWin);
    pView->pWin = NULL;

    SDL_Quit();
}

void startClient(ClientView *pView, ClientControl *pControl){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error. File: %s, Line: %d\n", __FILE__, __LINE__);
        return;
    }

    pView->windowHeight = 600;
    pView->windowWidth = 800;
    pView->pWin = SDL_CreateWindow("main", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, pView->windowWidth,pView->windowHeight,0);
    if(!pView->pWin) {
        printf("SDL_CreateWindow Error. File: %s, Line: %d\n", __FILE__, __LINE__);
        SDL_Quit();
        return;
    }

    pView->pRend = SDL_CreateRenderer(pView->pWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!pView->pRend) {
        printf("SDL_CreateRenderer Error. File: %s, Line: %d\n", __FILE__, __LINE__);
        SDL_DestroyWindow(pView->pWin);
        SDL_Quit();
        return;
    }

    pControl->isRunning = true;
}