#include "../include/game.h"
#include "../include/menu.h"

void gameLoop(ClientControl *pControl, ClientView *pView){
    Panel aPanel = initMenu(pView->pRend, pView);
    while (pControl->isRunning){
        eventHandler(pControl);
        updateMenu(aPanel, pControl);
        render(pView, aPanel);
    }

    UI_panelDestroy(aPanel);
}

void eventHandler(ClientControl *pControl){
    pControl->isMouseDown = false;
    pControl->isMouseUp = false;
    
    while (SDL_PollEvent(&pControl->event)){
        switch (pControl->event.type){
        case SDL_QUIT: pControl->isRunning = false;
            break;
        case SDL_KEYDOWN: pControl->keys[pControl->event.key.keysym.scancode] = true;
            break;
        case SDL_KEYUP: 
            pControl->keys[pControl->event.key.keysym.scancode] = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            pControl->isMouseDown = true;
            break;
        case SDL_MOUSEBUTTONUP:
            pControl->isMouseUp = true;
            break;
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
