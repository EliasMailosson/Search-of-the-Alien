#include "../include/game.h"
#include "../include/menu.h"

void gameLoop(ClientControl *pControl, ClientView *pView){
    Menu menu = initMenu(pView->pRend, pView);
    while (pControl->isRunning){
        eventHandler(pControl);
        updateMenu(&menu, pControl);
        render(pView, &menu);
    }

    destroyMenu(&menu);
}

void eventHandler(ClientControl *pControl){
    pControl->isMouseDown = false;
    pControl->isMouseUp = false;
    pControl->isTextInput = false;
    
}

void render(ClientView *pView, Menu *pMenu){
    SDL_SetRenderDrawColor(pView->pRend, 0,0,0,0);
    SDL_RenderClear(pView->pRend);
    renderMenu(pView->pRend, pMenu);
    SDL_RenderPresent(pView->pRend);
}
