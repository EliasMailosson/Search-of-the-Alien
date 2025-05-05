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
        // pMenu->isGameStarted = false;
        NET_clientSendInt(aClient, MENU, CHANGE_GAME_STATE, MENU);
    } 
    if(UI_buttonIsHovered((Button)pPauseMenu->button[1], mouseX, mouseY) && pControl->isMouseUp) {
    
    }
    if(UI_buttonIsHovered((Button)pPauseMenu->button[2], mouseX, mouseY) && pControl->isMouseUp) {
    
    }

    if (pControl->keys[SDL_SCANCODE_ESCAPE] && *pDelay > 12){
        NET_clientSendInt(aClient, LOBBY, PAUSE_MENU_REQUEST, 1);
        *pDelay = 0;
    }
}

void refreshPauseMenu(ClientView *pView, PauseMenu *pPm){
    int centerX = pView->windowWidth / 2;
    int centerY = pView->windowHeight / 2;

    UI_buttonConfigure(pPm->button[0], "Main Menu",
        pView->windowWidth / 2 - 150, pView->windowHeight * 0.65, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pView->pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pView->fonts, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    UI_buttonConfigure(pPm->button[1], "Options",
        pView->windowWidth / 2 - 150, pView->windowHeight * 0.50, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pView->pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pView->fonts, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    UI_buttonConfigure(pPm->button[2], "Back to Lobby",
        pView->windowWidth / 2 - 150, pView->windowHeight * 0.35, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pView->pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pView->fonts, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    ); 
}

void destroyPauseMenu(PauseMenu *pPauseMenu){
    for (int i = 0; i < MAX_BUTTONS_PAUSE_MENU; i++){
        UI_buttonDestroy(pPauseMenu->button[i]);
    }   
}