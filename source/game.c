#include "../include/game.h"
#include "../include/menu.h"

void gameLoop(Client aClient, ClientControl *pControl, ClientView *pView){
    NET_clientConnect(aClient);
    NET_clientSendString(aClient,MENU,CONNECT,"Jonatan");

    Menu menu = initMenu(pView->pRend, pView);
    Map aMap = MAP_CreateMap(pView->pRend, pView->windowWidth, pView->windowHeight);
    while (pControl->isRunning){
        eventHandler(pControl);

        switch (NET_clientGetState(aClient))
        {
        case MENU:
            runMenu(aClient, pControl, pView, &menu);
            break;
        case LOBBY:
            runMap(aClient, pControl, pView, aMap);
            break;
        default:
            break;
        }
    }

    destroyMenu(&menu);
    MAP_DestroyMap(aMap);
    NET_clientSendString(aClient,MENU,DISCONNECT,"Jonatan");
}

void runMenu(Client aClient, ClientControl *pControl, ClientView *pView, Menu *pMenu) {
    static int toggleDelay = 0;
    toggleDelay++;
    if(pControl->keys[SDL_SCANCODE_F11] && toggleDelay > 12) {
        toggleFullscreen(pView);
        refreshMenu(pView->pRend, pMenu, pView);
        toggleDelay = 0;
    }

    updateMenu(pMenu, pControl);
    if (pMenu->isGameStarted) {
        NET_clientSendInt(aClient, MENU, CHANGE_GAME_STATE, LOBBY);
    }
    NET_clientReceiver(aClient);
    renderMenu(pView->pRend, pMenu);
}

void runMap(Client aClient, ClientControl *pControl, ClientView *pView, Map aMap) {
    MAP_RenderMap(pView->pRend, aMap);
    SDL_SetRenderDrawColor(pView->pRend, 255, 0, 0, 255);  // red
    SDL_RenderDrawLine(pView->pRend, 0, pView->windowHeight / 2, pView->windowWidth / 2, pView->windowHeight / 2);   // diagonal line
     SDL_RenderDrawLine(pView->pRend, pView->windowWidth / 2, 0, pView->windowWidth / 2, pView->windowHeight / 2);   // diagonal line
    SDL_RenderPresent(pView->pRend);

}

void toggleFullscreen(ClientView *pView) {
    pView->windowFullscreen = !pView->windowFullscreen;

    if(pView->windowFullscreen) {
        SDL_SetWindowFullscreen(pView->pWin, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else {
        SDL_SetWindowFullscreen(pView->pWin, 0);
    }

    SDL_GetWindowSize(pView->pWin, &pView->windowWidth, &pView->windowHeight);
}

void eventHandler(ClientControl *pControl){
    pControl->isMouseDown = false;
    pControl->isMouseUp = false;
    pControl->isTextInput = false;
    
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
        case SDL_TEXTINPUT:
            pControl->isTextInput = true;
            strcpy(pControl->textInput, pControl->event.text.text);
            break;
        default:
            break;
        }
    }
}

void render(ClientView *pView, Menu *pMenu){
    SDL_SetRenderDrawColor(pView->pRend, 0,0,0,0);
    SDL_RenderClear(pView->pRend);
    renderMenu(pView->pRend, pMenu);
    SDL_RenderPresent(pView->pRend);
}
