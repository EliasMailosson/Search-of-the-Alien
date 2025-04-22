#include <stdio.h>
#include "../include/players.h"

#define SPRITE_SIZE 256
#define RENDER_SIZE 128

void renderPlayers(Client aClient, ClientView *pView, SDL_Rect playerCamera) {
    static int frame = 0;
    frame++;
    int playerCount = NET_clientGetPlayerCount(aClient);

    int selfIndex = NET_clientGetSelfIndex(aClient);
    SDL_Point selfPos = NET_clientGetPlayerPos(aClient, selfIndex);
    int centerX = pView->windowWidth/2;
    int centerY = pView->windowHeight/2;
    int renderSizeHalf = pView->playerRenderSize/2;

    // TODO: sort players in "y" value, so the players in front get rendered last.
    // for(int i = 0; i < playerCount; i++) {
    //     SDL_Point pos = NET_clientGetPlayerPos(aClient, i);
    // }

    for(int i = 0; i < playerCount; i++) {
        SDL_Point pos = NET_clientGetPlayerPos(aClient, i);
        int direction = NET_clientGetPlayerDirection(aClient, i);

        SDL_Rect playerRect;
        if(selfIndex == i) {
            playerRect = (SDL_Rect){
                .x = centerX - renderSizeHalf,
                .y = centerY - renderSizeHalf,
                .w = pView->playerRenderSize,
                .h = pView->playerRenderSize
            };
        }
        else {
            playerRect = (SDL_Rect){
                .x = centerX - (selfPos.x - pos.x) - renderSizeHalf,
                .y = centerY - (selfPos.y - pos.y) - renderSizeHalf,
                .w = pView->playerRenderSize,
                .h = pView->playerRenderSize
            };
        }
        pView->PlayerPos[i] = (SDL_Point){.x = playerRect.x, .y = playerRect.y}; 
        SDL_Rect src = {((frame/2)%24)*SPRITE_SIZE, direction*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
        SDL_RenderCopy(pView->pRend, pView->playerTexture, &src, &playerRect);
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