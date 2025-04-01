#include "../include/game.h"
#include "../include/menu.h"

void gameLoop(ClientControl *control, ClientView *view){
    Panel aPanel = initMenu();
    while (control->isRunning){
        eventHandler(control);
        render(view, aPanel);
    }

    free(aPanel);
    aPanel = NULL;
}

void eventHandler(ClientControl *pControl){
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

void render(ClientView *pView, Panel aPanel){
    SDL_SetRenderDrawColor(pView->pRend, 0,0,0,0);
    SDL_RenderClear(pView->pRend);
    renderMenu(pView->pRend, aPanel);
    SDL_RenderPresent(pView->pRend);
}
