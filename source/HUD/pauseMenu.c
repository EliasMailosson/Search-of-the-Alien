#include <stdio.h>
#include "../../include/HUD/pauseMenu.h"
#include "../../include/menu.h"

void initPauseMenu(ClientView *pView, PauseMenu *pPm){
    pPm->isVisible = false;

    for (int i = 0; i < MAX_BUTTONS_PAUSE_MENU; i++){
        pPm->button[i] = UI_buttonCreate();
    }

    refreshPauseMenu(pView, pPm);
}

void renderPauseMenu(ClientView *pView, PauseMenu *pPm) {
    SDL_SetRenderDrawBlendMode(pView->pRend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(pView->pRend, 0, 0, 0, 200); 
    SDL_RenderFillRect(pView->pRend, &((SDL_Rect){0,0,pView->windowWidth,pView->windowHeight}));

    for (int i = 0; i < MAX_BUTTONS_PAUSE_MENU; i++) {
        UI_buttonRenderer(pView->pRend, pPm->button[i]);
    }
}

void updatePauseMenu(PauseMenu *pPauseMenu, Client aClient, ClientControl *pControl, int *pDelay, Menu *pMenu){
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if(UI_buttonIsHovered((Button)pPauseMenu->button[0], mouseX, mouseY) && pControl->isMouseUp) {
        pMenu->currentPanel = PANEL_START;
        pMenu->isGameStarted = false;
        NET_clientSendInt(aClient, MENU, CHANGE_GAME_STATE, MENU);
        NET_clientSendInt(aClient, LOBBY, PAUSE_MENU_REQUEST, 1);

        for(int i = 0; i < PANEL_COUNT; i++) {
            UI_panelSetActive(pMenu->panels[i], (i == pMenu->currentPanel));
        }
    } 
    if(UI_buttonIsHovered((Button)pPauseMenu->button[1], mouseX, mouseY) && pControl->isMouseUp) {
        pMenu->currentPanel = PANEL_OPTIONS;
        pMenu->isGameStarted = false;
        NET_clientSendInt(aClient, MENU, CHANGE_GAME_STATE, MENU);
        NET_clientSendInt(aClient, LOBBY, PAUSE_MENU_REQUEST, 1);

        for(int i = 0; i < PANEL_COUNT; i++) {
            UI_panelSetActive(pMenu->panels[i], (i == pMenu->currentPanel));
        }
    }
    if(UI_buttonIsHovered((Button)pPauseMenu->button[2], mouseX, mouseY) && pControl->isMouseUp) {
        NET_clientSendInt(aClient, MENU, CHANGE_GAME_STATE, LOBBY);
        NET_clientSendInt(aClient, LOBBY, PAUSE_MENU_REQUEST, 1);
    }

    if (pControl->keys[SDL_SCANCODE_ESCAPE] && *pDelay > 12){
        NET_clientSendInt(aClient, LOBBY, PAUSE_MENU_REQUEST, 1);
        *pDelay = 0;
    }
}

void refreshPauseMenu(ClientView *pView, PauseMenu *pPm){
    int centerX = pView->windowWidth / 2;

    int firstRowY = pView->windowHeight * 0.45;

    UI_buttonConfigure(pPm->button[0], "Main Menu",
        centerX - 205, firstRowY, SMALLBUTTONWIDTH, BIGBUTTONHEIGHT, pView->pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pView->fonts, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 200 }
    );
    UI_buttonConfigure(pPm->button[1], "Options",
        centerX + 5, firstRowY, SMALLBUTTONWIDTH, BIGBUTTONHEIGHT, pView->pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pView->fonts, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 200 }
    );
    UI_buttonConfigure(pPm->button[2], "Back to Lobby",
        centerX - 205, firstRowY + 70, 410, BIGBUTTONHEIGHT, pView->pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pView->fonts, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 200 }
    ); 
}

void destroyPauseMenu(PauseMenu *pPauseMenu){
    for (int i = 0; i < MAX_BUTTONS_PAUSE_MENU; i++){
        UI_buttonDestroy(pPauseMenu->button[i]);
    }   
}