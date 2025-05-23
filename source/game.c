#include "../include/game.h"
#include "../include/entities.h"
#include "../include/UI/friend.h"
#include "../include/HUD/terminalHub.h"
#include "../include/HUD/pauseMenu.h"
#include "../include/menu.h"

static void enableMouseTexture(SDL_Cursor *CurrentCursor);
static void updatePositioning(Client aClient, SDL_Point lastPosition[MAX_CLIENTS], SDL_Point *playerPos, int selfIndex);
static void lobbyTerminalHubToggle(ClientControl *pControl, int *pDelay, Client aClient);
static void handlePlayerInput(Client aClient, ClientControl *pControl, ClientView *pView, int *pDelay);
static void updatePlayerAnimation(Client aClient, SDL_Point lastPosition[]);
static void renderLobby(ClientView *pView, Map aMap, Client aClient, TerminalHub terminalHub);
static void lobbyFullscreenToggle(Client aClient, ClientControl *pControl, ClientView *pView, Map aMap, int *pDelay, TerminalHub *pTerminalHub, PauseMenu *pPauseMenu, Menu *pMenu);
static void planetFullscreenToggle(Client aClient, ClientControl *pControl, ClientView *pView, Map aMap, int *pDelay, PauseMenu *pPauseMenu, TerminalHub *pTerminalHub, Menu *pMenu);

void gameLoop(Client aClient, ClientControl *pControl, ClientView *pView){
    NET_clientConnect(aClient);

    SOUND_setVolume(8, 64);
    SOUND_setMixVolume(pView->aSound);

    Menu menu = initMenu(pView->pRend, pView, aClient);

    TerminalHub terminalHub;
    initTerminalHub(pView, &terminalHub);

    PauseMenu pauseMenu;
    initPauseMenu(pView, &pauseMenu);
    
    char username[MAX_USERNAME_LEN];
    NET_clientGetSelfname(aClient, username);

    if(NET_clientIsUsernameSet(aClient)) {
        NET_clientSendString(aClient,MENU,CONNECT,username);
    }

    Mix_PlayChannel(-1, pView->aSound->vine, 0);
    
    Map aMap = MAP_MapCreate(pView->pRend, pView->windowWidth, pView->windowHeight);
    while (pControl->isRunning){
        eventHandler(pControl);

        switch (NET_clientGetState(aClient)){
        case MENU:
            runMenu(aClient, pControl, pView, &menu, aMap, &terminalHub, &pauseMenu);
            break;
        case LOBBY:
            runLobby(aClient, aMap, pControl, pView, &terminalHub, &pauseMenu, &menu);
            break;
        case NEMUR:
            runPlanet(aClient,pControl,pView,aMap, &pauseMenu, &menu, &terminalHub);
            break;
        default:
            break;
        }
    }

    destroyTerminalHub(&terminalHub);
    destroyPauseMenu(&pauseMenu);
    destroyMenu(&menu);
    MAP_MapDestroy(aMap);
    NET_clientGetSelfname(aClient, username);
    if(NET_clientIsUsernameSet(aClient)) {
        NET_clientSendString(aClient,MENU,DISCONNECT,username);
    }
}

void runLobby(Client aClient, Map aMap, ClientControl *pControl, ClientView *pView, TerminalHub *pTerminalHub, PauseMenu *pPauseMenu, Menu *pMenu) {
    static int toggleDelay = 0;
    int selfIndex = NET_clientGetSelfIndex(aClient);
    SDL_Point lastPosition[MAX_CLIENTS];
    SDL_Point playerPos;

    SOUND_playMusicIfChanged(pView->aSound, MUSIC_MENU);

    for (int i = 0; i < NET_clientGetEnemiesCount(aClient); i++)
    {
        SDL_Point point = NET_clientGetEnemyPos(aClient, i);
        // printf("enemy #%d: pos.x: %d\n pos.y: %d", i, point.x, point.y);
    }

    enableMouseTexture(pView->crosshair);
    updatePositioning(aClient, lastPosition, &playerPos, selfIndex);

    toggleDelay++;
    
    lobbyFullscreenToggle(aClient, pControl, pView, aMap, &toggleDelay, pTerminalHub, pPauseMenu, pMenu);


    pTerminalHub->isVisible = NET_clientGetTerminalHub(aClient);
    // printf("%d\n", pTerminalHub->isVisible);

    if (!pTerminalHub->isVisible){
        handlePlayerInput(aClient, pControl, pView, &toggleDelay);
    }
    else {
        updateTerminalHub(pTerminalHub, aClient, pControl->isMouseUp);
        lobbyTerminalHubToggle(pControl, &toggleDelay, aClient);
    }

    NET_clientReceiver(aClient,aMap,pView->pWin, pView->aSound);
    updatePlayerAnimation(aClient, lastPosition);

    // update HUD?
    updateArrows(pView->aHud,pView->pWin,aClient,pView->PlayerPos);
    if(toggleDelay%20 == 0) updateHudPlayerList(aClient, pView->aHud, pView->pRend, pView->windowWidth, pView->windowHeight);
    SDL_Rect tileRect = MAP_getTileRect(aMap);
    pView->playerRenderSize = tileRect.h;

    SDL_Point mapMovePos = {(playerPos.x/(float)TILE_SIZE)*tileRect.w - pView->windowWidth/2 - pView->playerRenderSize/2 + tileRect.w/2, (playerPos.y/(float)TILE_SIZE)*tileRect.h - pView->windowHeight/2 - pView->playerRenderSize + tileRect.h/2};
    MAP_MapMoveMap(aMap, mapMovePos);

    if(pControl->keys[SDL_SCANCODE_SPACE]){
        SOUND_playDash(pView->aSound);
    }

    renderLobby(pView, aMap, aClient, *pTerminalHub);
}

void runMenu(Client aClient, ClientControl *pControl, ClientView *pView, Menu *pMenu,Map aMap, TerminalHub *pTerminalHub, PauseMenu *pPauseMenu) {
    static int toggleDelay = 0;
    toggleDelay++;
    if(pControl->keys[SDL_SCANCODE_F] && toggleDelay > 12) {
        toggleFullscreen(pView);
        MAP_MapRefresh(aMap, pView->windowWidth, pView->windowHeight);
        refreshTerminalHub(pView, pTerminalHub);
        refreshPauseMenu(pView, pPauseMenu);
        updateHudPlayerList(aClient, pView->aHud, pView->pRend, pView->windowWidth, pView->windowHeight);
        refreshMenu(aClient, pView->pRend, pMenu, pView);
        toggleDelay = 0;
    }
    SOUND_playMusicIfChanged(pView->aSound, MUSIC_MENU);

    // if (pControl->isMouseDown && pMenu->currentPanel == PANEL_FRIENDS) {
    //     UI_FriendNameToggle(pMenu->friendList, pControl->mousePos.x, pControl->mousePos.y);
    // }    
    
    updateMenu(pMenu, pControl, pView, aClient, pView->aSound);
    if (pMenu->isGameStarted) {
        updateHudPlayerList(aClient, pView->aHud, pView->pRend, pView->windowWidth, pView->windowHeight);
        NET_clientSendInt(aClient, MENU, CHANGE_GAME_STATE, LOBBY);
    }
    NET_clientReceiver(aClient,aMap,pView->pWin, pView->aSound);
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

    //MAP_MapNewRender(pView->pRend, aMap, pView->pWin);
    MAP_MapRenderLobby(pView->pRend,aMap);
    
    // renderPlayers(aClient, pView);
    renderEntities(aClient, pView);
    hudRender(aClient, pView->aHud,pView->pRend, pView->windowWidth, pView->windowHeight);

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

static void lobbyFullscreenToggle(Client aClient, ClientControl *pControl, ClientView *pView, Map aMap, int *pDelay, TerminalHub *pTerminalHub, PauseMenu *pPauseMenu, Menu *pMenu) {
    if (pControl->keys[SDL_SCANCODE_F] && *pDelay > 12) {
        toggleFullscreen(pView);
        MAP_MapRefresh(aMap, pView->windowWidth, pView->windowHeight);
        refreshTerminalHub(pView, pTerminalHub);
        refreshPauseMenu(pView, pPauseMenu);
        updateHudPlayerList(aClient, pView->aHud, pView->pRend, pView->windowWidth, pView->windowHeight);
        refreshMenu(aClient, pView->pRend, pMenu, pView);
        *pDelay = 0;
    }
}

static void planetFullscreenToggle(Client aClient, ClientControl *pControl, ClientView *pView, Map aMap, int *pDelay, PauseMenu *pPauseMenu, TerminalHub *pTerminalHub, Menu *pMenu){
    if (pControl->keys[SDL_SCANCODE_F] && *pDelay > 12) {
        toggleFullscreen(pView);
        MAP_MapRefresh(aMap, pView->windowWidth, pView->windowHeight);
        refreshTerminalHub(pView, pTerminalHub);
        refreshPauseMenu(pView, pPauseMenu);
        updateHudPlayerList(aClient, pView->aHud, pView->pRend, pView->windowWidth, pView->windowHeight);
        refreshMenu(aClient, pView->pRend, pMenu, pView);
        *pDelay = 0;
    }
}

static void lobbyTerminalHubToggle(ClientControl *pControl, int *pDelay, Client aClient){
    if (pControl->keys[SDL_SCANCODE_E] && *pDelay > 12){
        NET_clientSendInt(aClient, LOBBY, TRY_OPEN_TERMINAL_HUB_RESPONSE, 1);
        *pDelay = 0;
    }
}

static void handlePlayerInput(Client aClient, ClientControl *pControl, ClientView *pView, int *pDelay) {
    PlayerInputPacket pip = prepareInputArray(pControl, pView->windowWidth, pView->windowHeight);

    if (NET_playerInputPacketCheck(pip)) {
        NET_clientSendArray(aClient, LOBBY, PLAYER_INPUT, &pip, sizeof(PlayerInputPacket));
    }
    if (pControl->keys[SDL_SCANCODE_E] && *pDelay > 12){
        NET_clientSendInt(aClient, LOBBY, TRY_OPEN_TERMINAL_HUB_RESPONSE, 1);
        *pDelay = 0;
    }
    else if (pControl->keys[SDL_SCANCODE_ESCAPE] && *pDelay > 12){
        NET_clientSendInt(aClient, LOBBY, PAUSE_MENU_REQUEST, 1);
        *pDelay = 0;
    }
}

void renderPlanet(ClientView *pView, Map aMap, Client aClient, PauseMenu *pPauseMenu){
    SDL_SetRenderDrawColor(pView->pRend, 0,0,0,0);
    SDL_RenderClear(pView->pRend);

    MAP_MapNewRender(pView->pRend, aMap, pView->pWin);
    renderProjectiles(aClient, pView);

    renderEntities(aClient, pView);
    
    if (pPauseMenu->isVisible) {
        renderPauseMenu(pView, pPauseMenu);
    }
    

    hudRender(aClient, pView->aHud, pView->pRend, pView->windowWidth, pView->windowHeight);
    SDL_RenderPresent(pView->pRend);
}

void runPlanet(Client aClient, ClientControl *pControl, ClientView *pView, Map aMap, PauseMenu *pPauseMenu, Menu *pMenu, TerminalHub *pTerminalHub){
    static int toggleDelay = 0;
    int selfIndex = NET_clientGetSelfIndex(aClient);
    SDL_Point lastPosition[MAX_CLIENTS];
    SDL_Point playerPos;

    SOUND_playMusicIfChanged(pView->aSound, MUSIC_NEMUR);

    enableMouseTexture(pView->crosshair);
    updatePositioning(aClient, lastPosition, &playerPos, selfIndex);
    if(!pPauseMenu->isVisible) {
        handlePlayerInput(aClient, pControl, pView, &toggleDelay);
    } else {
        updatePauseMenu(pPauseMenu, aClient, pControl, &toggleDelay, pMenu);
    }
    
    planetFullscreenToggle(aClient, pControl,pView,aMap,&toggleDelay, pPauseMenu, pTerminalHub, pMenu);
    
    toggleDelay++;

    NET_clientReceiver(aClient,aMap,pView->pWin, pView->aSound);

    for(int i = 0; i < NET_clientGetEnemiesCount(aClient); i++){
        if(NET_clientIsEnemyDamaged(aClient, i)){
            SOUND_enemyIsHurt(pView->aSound);
            // printf("ENEMY HURT #%d!!\n", i);
        }
    }

    pPauseMenu->isVisible = NET_clientGetPauseState(aClient);

    updatePlayerAnimation(aClient, lastPosition);

    updateArrows(pView->aHud,pView->pWin,aClient,pView->PlayerPos);
    if(toggleDelay%20 == 0) updateHudPlayerList(aClient, pView->aHud, pView->pRend, pView->windowWidth, pView->windowHeight);
    SDL_Rect tileRect = MAP_getTileRect(aMap);
    pView->playerRenderSize = tileRect.h;

    SDL_Point mapMovePos = {
        (playerPos.x/(float)TILE_SIZE)*tileRect.w - pView->windowWidth/2 - pView->playerRenderSize/2 + tileRect.w/2,
        (playerPos.y/(float)TILE_SIZE)*tileRect.h - pView->windowHeight/2 - pView->playerRenderSize + tileRect.h/2};
    MAP_MapMoveMap(aMap, mapMovePos);

    if(pControl->keys[SDL_SCANCODE_SPACE]){
        SOUND_playDash(pView->aSound);
    }
    
    renderPlanet(pView,aMap,aClient,pPauseMenu);
}