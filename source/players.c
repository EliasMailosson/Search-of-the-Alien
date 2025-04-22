#include <stdio.h>
#include<string.h>
#include "../include/players.h"
#include "../include/NET/client.h"
#include "../include/game.h"
#include "../include/clientLife.h"

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
        
        SDL_Rect src;
        switch(NET_clientGetPlayerAnimation(aClient, i)) {
            case ANIMATION_IDLE:
                src = (SDL_Rect){((frame/2)%24)*SPRITE_SIZE, (direction+8)*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
                break;
            case ANIMATION_RUNNING:
                src = (SDL_Rect){((frame/2)%24)*SPRITE_SIZE, direction*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
                break;
            default:
                src = (SDL_Rect){((frame/2)%24)*SPRITE_SIZE, direction*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
        }
        
        SDL_RenderCopy(pView->pRend, pView->playerTexture, &src, &playerRect);
        RenderPlayerName(aClient, pView, i, playerRect);
    }
}

PlayerInputPacket prepareInputArray(ClientControl *pControl, int windowWidth, int windowHeight) {
    PlayerInputPacket pip = {
        // Later: let mousePos x and y be float between 0.0 and 1.0, normalized to the screen size. 
        .mousePos = {
            .x = pControl->mousePos.x  - (windowWidth / 2),
            .y = pControl->mousePos.y - (windowHeight / 2)
        },
        .keys = {
            pControl->keys[SDL_SCANCODE_W],
            pControl->keys[SDL_SCANCODE_S],
            pControl->keys[SDL_SCANCODE_D],
            pControl->keys[SDL_SCANCODE_A],
            pControl->keys[SDL_SCANCODE_SPACE],
            pControl->isMouseDown,
            // pControl->isMouseUp, en för mycket
        }
    };
    return pip;
}

void updatepos(){

}