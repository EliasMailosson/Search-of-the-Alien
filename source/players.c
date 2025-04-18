#include <stdio.h>
#include "../include/players.h"

#define SPRITE_SIZE 256
#define RENDER_SIZE 128

void renderPlayers(Client aClient, ClientView *pView, SDL_Rect playerCamera) {
    static int frame = 0;
    frame++;
    int playerCount = NET_clientGetPlayerCount(aClient);

    for(int i = 0; i < playerCount; i++) {
        SDL_Point pos = NET_clientGetPlayerPos(aClient, i);
        int direction = NET_clientGetPlayerDirection(aClient, i);

        SDL_Rect playerRect;
        if(NET_clientGetSelfIndex(aClient) == i) {
            playerRect = (SDL_Rect){
                .x = pView->windowWidth/2 - 40,
                .y = pView->windowHeight/2 - 40,
                .w = 80,
                .h = 80
            };
        }
        else {
            playerRect = (SDL_Rect){
                .x = pos.x - playerCamera.x - 40,
                .y = pos.y - playerCamera.y - 40,
                .w = 80,
                .h = 80
            };
        }
        
        SDL_SetRenderDrawColor(pView->pRend, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        SDL_RenderFillRect(pView->pRend, &playerRect);
    }
}

PlayerInputPacket prepareInputArray(ClientControl *pControl) {
    PlayerInputPacket pip = {
        .mousePos = {
            .x = pControl->mousePos.x,
            .y = pControl->mousePos.y 
        },
        .keys = {
            pControl->keys[SDL_SCANCODE_W],
            pControl->keys[SDL_SCANCODE_S],
            pControl->keys[SDL_SCANCODE_D],
            pControl->keys[SDL_SCANCODE_A],
            pControl->keys[SDL_SCANCODE_SPACE],
            pControl->isMouseDown,
            pControl->isMouseUp
        }
    };
    return pip;
}

void updatepos(){

}