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
            runLobby(aClient, pControl, pView);
            runMap(aClient, pControl, pView, aMap);
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

void runLobby(Client aClient, ClientControl *pControl, ClientView *pView) {
    static int toggleDelay = 0;
    toggleDelay++;
    if(pControl->keys[SDL_SCANCODE_F] && toggleDelay > 12) {
        toggleFullscreen(pView);
        toggleDelay = 0;
    }

    PlayerInputPacket pip;
    pip = prepareInputArray(pControl);
    if(NET_playerInputPacketCheck(pip)){
        NET_clientSendArray(aClient, LOBBY, PLAYER_INPUT, &pip, sizeof(PlayerInputPacket));
    }

    NET_clientReceiver(aClient);
    
    renderPlayers(aClient, pView);
    
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

void runMap(Client aClient, ClientControl *pControl, ClientView *pView, Map aMap) {
    static int toggleDelay = 0;
    toggleDelay++;
    if(pControl->keys[SDL_SCANCODE_F] && toggleDelay > 12) {
        toggleFullscreen(pView);
        MAP_refreshMap(aMap, pView->windowWidth, pView->windowHeight);
        toggleDelay = 0;
    }
    MAP_RenderMap(pView->pRend, aMap);
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
