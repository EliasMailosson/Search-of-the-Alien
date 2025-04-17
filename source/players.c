#include <stdio.h>
#include "../include/players.h"

void renderPlayers(Client aClient, ClientView *pView) {
    int playerCount = NET_clientGetPlayerCount(aClient);

    SDL_Color colors[MAX_CLIENTS] = {
        {.r = 255, .g = 0, .b = 0, .a = 0},
        {.r = 0, .g = 255, .b = 0, .a = 0},
        {.r = 0, .g = 0, .b = 255, .a = 0},
        {.r = 255, .g = 255, .b = 0, .a = 0},
        {.r = 255, .g = 0, .b = 255, .a = 0},
        {.r = 0, .g = 255, .b = 255, .a = 0},
        {.r = 255, .g = 255, .b = 255, .a = 0},
        {.r = 100, .g = 100, .b = 100, .a = 0}
    };

    for(int i = 0; i < playerCount; i++) {
        SDL_Point pos = NET_clientGetPlayerPos(aClient, i);
        SDL_Rect playerRect = {pos.x, pos.y, 80, 80};

        char name[MAX_USERNAME_LEN];
        NET_clientGetSelfname(aClient,name);
        if(NET_clientFindPlayer(aClient, name) == i){
            SDL_Rect center = (SDL_Rect){pView->windowWidth/2-40,pView->windowHeight/2-80,80,80};
            SDL_SetRenderDrawColor(pView->pRend, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
            SDL_RenderFillRect(pView->pRend, &center);
            continue;
        }
        SDL_SetRenderDrawColor(pView->pRend, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        SDL_RenderFillRect(pView->pRend, &playerRect);
    }
}

PlayerInputPacket prepareInputArray(ClientControl *pControl) {
    PlayerInputPacket pip = {
        .keys = {
            pControl->keys[SDL_SCANCODE_W],
            pControl->keys[SDL_SCANCODE_S],
            pControl->keys[SDL_SCANCODE_D],
            pControl->keys[SDL_SCANCODE_A],
            pControl->isMouseDown,
            pControl->isMouseUp
        }
    };
    return pip;
}

void updatepos(){

}