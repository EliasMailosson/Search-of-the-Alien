#include "../include/game.h"
#include "../include/menu.h"
#include "../include/players.h"
#include "../include/UI/friend.h"

static void enableMouseTexture(SDL_Cursor *CurrentCursor);
static void updatePositioning(Client aClient, SDL_Point lastPosition[MAX_CLIENTS], SDL_Point *playerPos, int selfIndex);
static void lobbyFullscreenToggle(ClientControl *pControl, ClientView *pView, Map aMap, int *pDelay);
static void lobbyTerminalHubToggle(ClientControl *pControl, bool *pShowHub, int *pDelay);
static void handlePlayerInput(Client aClient, ClientControl *pControl, ClientView *pView);
static void updatePlayerAnimation(Client aClient, SDL_Point lastPosition[]);
static void renderLobby(ClientView *pView, Map aMap, Client aClient, bool showHub);


void gameLoop(Client aClient, ClientControl *pControl, ClientView *pView){
    NET_clientConnect(aClient);

    Menu menu = initMenu(pView->pRend, pView, aClient);

    PlanetChooser planetChooser = initPlanetChooser();
    
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
            runLobby(aClient, aMap, pControl, pView, &planetChooser);
            break;
        default:
            break;
        }
    }

    // destroyPlanetChooser(&planetChooser);
    destroyMenu(&menu);
    MAP_MapDestroy(aMap);
    NET_clientGetSelfname(aClient, username);
    if(strcmp(username, "None") != 0) {
        NET_clientSendString(aClient,MENU,DISCONNECT,username);
    }
}

void runLobby(Client aClient, Map aMap, ClientControl *pControl, ClientView *pView, PlanetChooser *pPlanetChooser) {
    static int toggleDelay = 0;
    int selfIndex = NET_clientGetSelfIndex(aClient);
    SDL_Point lastPosition[MAX_CLIENTS];
    SDL_Point playerPos;

    enableMouseTexture(pView->crosshair);
    updatePositioning(aClient, lastPosition, &playerPos, selfIndex);

    toggleDelay++;
    
    lobbyFullscreenToggle(pControl, pView, aMap, &toggleDelay);
    lobbyTerminalHubToggle(pControl, &pPlanetChooser->isVisible, &toggleDelay);

    if (!pPlanetChooser->isVisible){
        handlePlayerInput(aClient, pControl, pView);
    }

    NET_clientReceiver(aClient);
    updatePlayerAnimation(aClient, lastPosition);

    // update HUD?
    updateArrows(pView->aHud,pView->pWin,aClient,pView->PlayerPos);
    SDL_Rect tileRect = MAP_getTileRect(aMap);
    pView->playerRenderSize = tileRect.h;

    SDL_Point mapMovePos = {(playerPos.x/(float)TILE_SIZE)*tileRect.w, (playerPos.y/(float)TILE_SIZE)*tileRect.h};
    MAP_MapMoveMap(aMap, mapMovePos);

    renderLobby(pView, aMap, aClient, pPlanetChooser->isVisible);
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

static void renderLobby(ClientView *pView, Map aMap, Client aClient, bool showHub){
    SDL_SetRenderDrawColor(pView->pRend, 0,0,0,0);
    SDL_RenderClear(pView->pRend);

    MAP_MapRender(pView->pRend, aMap);
    renderPlayers(aClient, pView);
    for (int i = 0; i < NET_clientGetPlayerCount(aClient); i++){
        hudRender(pView->aHud,pView->pRend,NET_clientGetPlayerColorIndex(aClient,i),i);
    }
    if (showHub) {
        renderPlanetChooser(pView);
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

static void lobbyFullscreenToggle(ClientControl *pControl, ClientView *pView, Map aMap, int *pDelay) {
    if (pControl->keys[SDL_SCANCODE_F] && *pDelay > 12) {
        toggleFullscreen(pView);
        MAP_MapRefresh(aMap, pView->windowWidth, pView->windowHeight);
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


// struct planetChooser --> is visible , Button buttons[]

// void createPlanetChooser() // skapar knappar och sätter start värden -- returnarer struct 

// void destroyPlanetChooser() destroy all buttons

// void updatePlanetChooser() {
    

// }// kollar input på knappar och uppdaterar pControl
// // kolla om knapp är klivkad oh byt state

void renderPlanetChooser(ClientView *pView){ // ritar knappar och rektanglar... (pView)
    int centerX = pView->windowWidth / 2;
    int centerY = pView->windowHeight / 2;

    int hubWidth = (pView->windowWidth / 3) * 2 ;
    int hubHeight = (pView->windowHeight / 3) * 2;
    
    SDL_Rect hubRect = {
        .x = centerX - hubWidth / 2,
        .y = centerY - hubHeight / 2,
        .w = hubWidth,
        .h = hubHeight 
    };

// rita knappar

    SDL_SetRenderDrawBlendMode(pView->pRend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(pView->pRend, 255, 0, 0, 200); // semi-transparent red
    SDL_RenderFillRect(pView->pRend, &hubRect);
}

PlanetChooser initPlanetChooser(){
    PlanetChooser planetChooser;

    planetChooser.buttonCount = 3;
    planetChooser.isVisible = false;

    // for (int i = 0; i < planetChooser.buttonCount; i++){
    //     planetChooser.button[i] = UI_buttonCreate();
    // }

    return planetChooser;
}
void destroyPlanetChooser(PlanetChooser *pPlanetChooser){
    for (int i = 0; i < pPlanetChooser->buttonCount; i++)
    {
        UI_buttonDestroy(pPlanetChooser->button[i]);
    }   
}