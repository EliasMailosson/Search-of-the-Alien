#include "../include/game.h"
#include "../include/menu.h"
#include "../include/players.h"
#include "../include/UI/friend.h"

void gameLoop(Client aClient, ClientControl *pControl, ClientView *pView){
    NET_clientConnect(aClient);

    Menu menu = initMenu(pView->pRend, pView, aClient);
    
    char username[MAX_USERNAME_LEN];
    NET_clientGetSelfname(aClient, username);

    if(strcmp(username, "None") != 0) {
        NET_clientSendString(aClient,MENU,CONNECT,username);
    }
    
    Map aMap = MAP_MapCreate(pView->pRend, pView->windowWidth, pView->windowHeight);
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
    MAP_MapDestroy(aMap);
    NET_clientGetSelfname(aClient, username);
    if(strcmp(username, "None") != 0) {
        NET_clientSendString(aClient,MENU,DISCONNECT,username);
    }
}

void runLobby(Client aClient, Map aMap, ClientControl *pControl, ClientView *pView) {
    static int toggleDelay = 0;
    int selfIndex = NET_clientGetSelfIndex(aClient);

    SDL_Point lastPosition[MAX_CLIENTS];
    for(int i = 0; i < NET_clientGetPlayerCount(aClient); i++) {
        lastPosition[i] = NET_clientGetPlayerPos(aClient, i);
    }

    SDL_Point playerPos = NET_clientGetPlayerPos(aClient, selfIndex);

    SDL_Rect playerCamera = {
        .x = playerPos.x - pView->windowWidth/2,
        .y = playerPos.y - pView->windowHeight/2,
        .w = pView->windowWidth,
        .h = pView->windowHeight
    };

    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetCursor(pView->crosshair);
    SDL_ShowCursor(SDL_ENABLE);

    toggleDelay++;
    if(pControl->keys[SDL_SCANCODE_F] && toggleDelay > 12) {
        toggleFullscreen(pView);
        MAP_MapRefresh(aMap, pView->windowWidth, pView->windowHeight);
        toggleDelay = 0;
    }

    MAP_MapRefresh(aMap, pView->windowWidth, pView->windowHeight);

    SDL_Rect tileRect = MAP_getTileRect(aMap);
    pView->playerRenderSize = tileRect.h;

    PlayerInputPacket pip;
    pip = prepareInputArray(pControl, pView->windowWidth, pView->windowHeight);
    if(NET_playerInputPacketCheck(pip)){
        NET_clientSendArray(aClient, LOBBY, PLAYER_INPUT, &pip, sizeof(PlayerInputPacket));
    }

    NET_clientReceiver(aClient);
    
    
    for(int i = 0; i < NET_clientGetPlayerCount(aClient); i++) {
        SDL_Point newPosition = NET_clientGetPlayerPos(aClient, i);
        int dx = abs(newPosition.x - lastPosition[i].x);
        int dy = abs(newPosition.y - lastPosition[i].y);
        if(dx > 0 || dy > 0) {
            NET_clientSetPlayerAnimation(aClient, i, ANIMATION_RUNNING);
        }
        else {
            NET_clientSetPlayerAnimation(aClient, i, ANIMATION_IDLE);
        }
    }
    updateArrows(pView->aHud,pView->pWin,aClient,pView->PlayerPos);
    MAP_MapMoveMap(aMap, playerPos);

    SDL_SetRenderDrawColor(pView->pRend, 0,0,0,0);
    SDL_RenderClear(pView->pRend);

    MAP_MapRender(pView->pRend, aMap);
    renderPlayers(aClient, pView, playerCamera);
    for (int i = 0; i < NET_clientGetPlayerCount(aClient); i++){
        hudRender(pView->aHud,pView->pRend,i,i);
    }
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

    // if (pControl->isMouseDown && pMenu->currentPanel == PANEL_FRIENDS) {
    //     UI_FriendNameToggle(pMenu->friendList, pControl->mousePos.x, pControl->mousePos.y);
    // }    
    
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
