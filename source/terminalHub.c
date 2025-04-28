#include <stdio.h>

#include "../include/terminalHub.h"
#include "../include/menu.h"
#include "../include/game.h"

void renderTerminalHub(ClientView *pView, TerminalHub terminalHub){ // ritar knappar och rektanglar... (pView)
    int centerX = pView->windowWidth / 2;
    int centerY = pView->windowHeight / 2;
    
    SDL_Rect hubRect = {
        .x = centerX - terminalHub.hubWidth / 2,
        .y = centerY - terminalHub.hubHeight / 2,
        .w = terminalHub.hubWidth,
        .h = terminalHub.hubHeight 
    };

    SDL_SetRenderDrawBlendMode(pView->pRend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(pView->pRend, 255, 0, 0, 200); // semi-transparent red
    SDL_RenderFillRect(pView->pRend, &hubRect);

    for (int i = 0; i < MAX_BUTTONS_TERMINAL_HUB; i++) {
        UI_buttonRenderer(pView->pRend, terminalHub.button[i]);
    }
}

TerminalHub initTerminalHub(ClientView *pView){
    TerminalHub terminalHub;
    terminalHub.isVisible = false;

    for (int i = 0; i < MAX_BUTTONS_TERMINAL_HUB; i++){
        terminalHub.button[i] = UI_buttonCreate();
    }

    terminalHub.hubWidth  = (pView->windowWidth / 3) * 2 ;
    terminalHub.hubHeight = (pView->windowHeight / 3) * 2;

    UI_buttonConfigure(terminalHub.button[0], "Cindora", 
        pView->windowWidth / 2 - 150, pView->windowHeight * 0.65, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pView->pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pView->fonts, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    UI_buttonConfigure(terminalHub.button[1], "Aurantic", 
        pView->windowWidth / 2 - 150, pView->windowHeight * 0.50, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pView->pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pView->fonts, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    UI_buttonConfigure(terminalHub.button[2], "Nemur", 
        pView->windowWidth / 2 - 150, pView->windowHeight * 0.35, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pView->pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pView->fonts, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    ); 

    UI_buttonConfigure(terminalHub.button[3], "Lobby", 
        pView->windowWidth / 2 - 150, pView->windowHeight * 0.20, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pView->pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pView->fonts, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    ); 

    return terminalHub;
}
void destroyTerminalHub(TerminalHub *pTerminalHub){
    for (int i = 0; i < MAX_BUTTONS_TERMINAL_HUB; i++){
        UI_buttonDestroy(pTerminalHub->button[i]);
    }   
}

void updateTerminalHub(TerminalHub *pTerminalHub, Client aClient, bool isMouseUp){
    char text[MAX_USERNAME_LEN];
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    for (int i = 0; i < MAX_BUTTONS_TERMINAL_HUB; i++)
    {
        if(UI_buttonIsHovered((Button)pTerminalHub->button[i], mouseX, mouseY)) {
            if(isMouseUp) {
                UI_buttonGetText(pTerminalHub->button[i], text);
                
                if (strcmp(text, "Nemur") == 0){
                    printf("Nemur\n");
                    NET_clientSendInt(aClient, NET_clientGetState(aClient), CHANGE_GAME_STATE, NEMUR);
                }

                if (strcmp(text, "Aurantic") == 0){
                    // NET_clientSendInt(aClient, NET_clientGetState(aClient), CHANGE_GAME_STATE, AURANTIC);
                }

                if (strcmp(text, "Cindora") == 0){
                    // NET_clientSendInt(aClient, NET_clientGetState(aClient), CHANGE_GAME_STATE, CINDORA);
                }

                if (strcmp(text, "Lobby") == 0){
                    // NET_clientSendInt(aClient, NET_clientGetState(aClient), CHANGE_GAME_STATE, LOBBY);
                }
            }    
        }
    }
}