#include "../include/game.h"
#include "../include/menu.h"
#include "../include/players.h"

void gameLoop(Client aClient, ClientControl *pControl, ClientView *pView){
    NET_clientConnect(aClient);

    Menu menu = initMenu(pView->pRend, pView, aClient);
    char username[MAX_USERNAME_LEN];
    NET_getSelfname(aClient, username);

    if(strcmp(username, "None") != 0) {
        NET_clientSendString(aClient,MENU,CONNECT,username);
    }
    
    Map aMap = MAP_CreateMap(pView->pRend, pView->windowWidth, pView->windowHeight);
    while (pControl->isRunning){
        eventHandler(pControl);

        switch (NET_clientGetState(aClient))
        {
        case MENU:
            runMenu(aClient, pControl, pView, &menu);
            break;
        case LOBBY:
            runLobby(aClient, aMap, pControl, pView);
            break;
        default:
            break;
        }
    }

    destroyMenu(&menu);
    MAP_DestroyMap(aMap);
    NET_getSelfname(aClient, username);
    if(strcmp(username, "None") != 0) {
        NET_clientSendString(aClient,MENU,DISCONNECT,username);
    }
}

void runLobby(Client aClient, Map aMap, ClientControl *pControl, ClientView *pView) {
    static int toggleDelay = 0;

    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetCursor(pView->crosshair);
    SDL_ShowCursor(SDL_ENABLE);

    toggleDelay++;
    if(pControl->keys[SDL_SCANCODE_F] && toggleDelay > 12) {
        toggleFullscreen(pView);
        MAP_refreshMap(aMap, pView->windowWidth, pView->windowHeight);
        toggleDelay = 0;
    }

    PlayerInputPacket pip;
    pip = prepareInputArray(pControl);
    if(NET_playerInputPacketCheck(pip)){
        NET_clientSendArray(aClient, LOBBY, PLAYER_INPUT, &pip, sizeof(PlayerInputPacket));
    }

    NET_clientReceiver(aClient);
    
    SDL_SetRenderDrawColor(pView->pRend, 0,0,0,0);
    SDL_RenderClear(pView->pRend);

    MAP_RenderMap(pView->pRend, aMap);
    renderPlayers(aClient, pView);

    // SDL_Rect mouseTest = {.w = 16, .h = 16, .x = pControl->mousePos.x, .y = pControl->mousePos.y};
    // SDL_SetRenderDrawColor(pView->pRend, 255,255,0,0);
    // SDL_RenderFillRect(pView->pRend, &mouseTest);
    
    SDL_RenderPresent(pView->pRend);
}

void runMenu(Client aClient, ClientControl *pControl, ClientView *pView, Menu *pMenu) {
    static int toggleDelay = 0;
    toggleDelay++;
    if(pControl->keys[SDL_SCANCODE_F] && toggleDelay > 12) {
        toggleFullscreen(pView);
        refreshMenu(pView->pRend, pMenu, pView);
        toggleDelay = 0;
    }

    updateMenu(pMenu, pControl, aClient);
    if (pMenu->isGameStarted) {
        NET_clientSendInt(aClient, MENU, CHANGE_GAME_STATE, LOBBY);
    }
    NET_clientReceiver(aClient);
    renderMenu(pView->pRend, pMenu);
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
        SDL_GetMouseState(&pControl->mousePos.x, &pControl->mousePos.y);

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
