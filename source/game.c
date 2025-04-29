#include "../include/game.h"
#include "../include/players.h"
#include "../include/UI/friend.h"
#include "../include/terminalHub.h"
#include "../include/menu.h"

static void enableMouseTexture(SDL_Cursor *CurrentCursor);
static void updatePositioning(Client aClient, SDL_Point lastPosition[MAX_CLIENTS], SDL_Point *playerPos, int selfIndex);
static void lobbyFullscreenToggle(ClientControl *pControl, ClientView *pView, Map aMap, int *pDelay, TerminalHub *pTerminalHub);
static void lobbyTerminalHubToggle(ClientControl *pControl, bool *pShowHub, int *pDelay);
static void handlePlayerInput(Client aClient, ClientControl *pControl, ClientView *pView);
static void updatePlayerAnimation(Client aClient, SDL_Point lastPosition[]);
static void renderLobby(ClientView *pView, Map aMap, Client aClient, TerminalHub terminalHub);


void gameLoop(Client aClient, ClientControl *pControl, ClientView *pView){
    NET_clientConnect(aClient);

    Menu menu = initMenu(pView->pRend, pView, aClient);

    TerminalHub terminalHub;
    initTerminalHub(pView, &terminalHub);
    
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
            runLobby(aClient, aMap, pControl, pView, &terminalHub);
            break;
        case NEMUR:
            runNemur();
            break;
        default:
            break;
        }
    }

    destroyTerminalHub(&terminalHub);
    destroyMenu(&menu);
    MAP_MapDestroy(aMap);
    NET_clientGetSelfname(aClient, username);
    if(strcmp(username, "None") != 0) {
        NET_clientSendString(aClient,MENU,DISCONNECT,username);
    }
}

void runLobby(Client aClient, Map aMap, ClientControl *pControl, ClientView *pView, TerminalHub *pTerminalHub) {
    static int toggleDelay = 0;
    int selfIndex = NET_clientGetSelfIndex(aClient);
    SDL_Point lastPosition[MAX_CLIENTS];
    SDL_Point playerPos;

    enableMouseTexture(pView->crosshair);
    updatePositioning(aClient, lastPosition, &playerPos, selfIndex);

    toggleDelay++;
    
    lobbyFullscreenToggle(pControl, pView, aMap, &toggleDelay, pTerminalHub);

    int tileX, tileY;
    MAP_PixelsToTile(aMap, playerPos.x, playerPos.y, &tileX, &tileY);


    if (tileX >= 10 && tileX <= 25 && tileY >= -1 && tileY <= 6 && pView->windowFullscreen){
        lobbyTerminalHubToggle(pControl, &pTerminalHub->isVisible, &toggleDelay);
    }
    else if (tileX >= 19 && tileX <= 40 && tileY >= 0 && tileY <= 11 && !pView->windowFullscreen){
        lobbyTerminalHubToggle(pControl, &pTerminalHub->isVisible, &toggleDelay);
    }
    
    
    if (!pTerminalHub->isVisible){
        handlePlayerInput(aClient, pControl, pView);
    }
    else {
        updateTerminalHub(pTerminalHub, aClient, pControl->isMouseUp);
    }

    NET_clientReceiver(aClient);
    updatePlayerAnimation(aClient, lastPosition);

    // update HUD?
    updateArrows(pView->aHud,pView->pWin,aClient,pView->PlayerPos);
    SDL_Rect tileRect = MAP_getTileRect(aMap);
    pView->playerRenderSize = tileRect.h;

    SDL_Point mapMovePos = {(playerPos.x/(float)TILE_SIZE)*tileRect.w - pView->windowWidth/2 - pView->playerRenderSize/2 + tileRect.w/2, (playerPos.y/(float)TILE_SIZE)*tileRect.h - pView->windowHeight/2 - pView->playerRenderSize + tileRect.h/2};
    MAP_MapMoveMap(aMap, mapMovePos);

    renderLobby(pView, aMap, aClient, *pTerminalHub);
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

static void renderLobby(ClientView *pView, Map aMap, Client aClient, TerminalHub terminalHub){
    SDL_SetRenderDrawColor(pView->pRend, 0,0,0,0);
    SDL_RenderClear(pView->pRend);

    MAP_MapRender(pView->pRend, aMap);
    renderPlayers(aClient, pView);
    for (int i = 0; i < NET_clientGetPlayerCount(aClient); i++){
        hudRender(pView->aHud,pView->pRend,NET_clientGetPlayerColorIndex(aClient,i),i);
    }
    if (terminalHub.isVisible) {
        renderTerminalHub(pView, terminalHub);
    }
    SDL_RenderPresent(pView->pRend);

}

static void updatePlayerAnimation(Client aClient, SDL_Point lastPosition[]){
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
}

/* Disables the previous cursor and sets new one 
 * depending on the SDL_Cursor Texture in argument
 */
static void enableMouseTexture(SDL_Cursor *CurrentCursor){
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetCursor(CurrentCursor);
    SDL_ShowCursor(SDL_ENABLE);
}

static void updatePositioning(Client aClient, SDL_Point lastPosition[MAX_CLIENTS], SDL_Point *playerPos, int selfIndex){
        for(int i = 0; i < NET_clientGetPlayerCount(aClient); i++) {
            lastPosition[i] = NET_clientGetPlayerPos(aClient, i);
        }
        *playerPos = NET_clientGetPlayerPos(aClient, selfIndex);
}

static void lobbyFullscreenToggle(ClientControl *pControl, ClientView *pView, Map aMap, int *pDelay, TerminalHub *pTerminalHub) {
    if (pControl->keys[SDL_SCANCODE_F] && *pDelay > 12) {
        toggleFullscreen(pView);
        MAP_MapRefresh(aMap, pView->windowWidth, pView->windowHeight);
        refreshTerminalHub(pView, pTerminalHub);
        *pDelay = 0;
    }
}

static void lobbyTerminalHubToggle(ClientControl *pControl, bool *pShowHub, int *pDelay){
    if (pControl->keys[SDL_SCANCODE_E] && *pDelay > 12){
        *pShowHub = !*pShowHub;
        *pDelay = 0;
    }
}


static void handlePlayerInput(Client aClient, ClientControl *pControl, ClientView *pView) {
    PlayerInputPacket pip = prepareInputArray(pControl, pView->windowWidth, pView->windowHeight);
    if (NET_playerInputPacketCheck(pip)) {
        NET_clientSendArray(aClient, LOBBY, PLAYER_INPUT, &pip, sizeof(PlayerInputPacket));
    }
}

void runNemur(){

}