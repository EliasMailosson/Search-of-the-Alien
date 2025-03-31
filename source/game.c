#include "game.h"

void gameLoop(ClientControl control, ClientView view){
    while (control.isRunning){
        eventHandel(&control);
        reder(&view);
    }
}

void eventHandel(ClientControl *pControl){
    while (SDL_PollEvent(&pControl->event)){
        switch (pControl->event.type){
        case SDL_QUIT: pControl->isRunning = false;
            break;
        case SDL_KEYDOWN: pControl->keys[pControl->event.key.keysym.scancode] = true;
            break;
        case SDL_KEYUP: pControl->keys[pControl->event.key.keysym.scancode] = false;
        default:
            break;
        }
    }
}

void reder(ClientView *pView){
    SDL_RenderClear(pView->pRend);

    SDL_RenderPresent(pView->pRend);
}
