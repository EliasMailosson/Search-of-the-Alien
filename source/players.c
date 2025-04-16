#include <stdio.h>
#include "../include/players.h"

#define SPRITE_SIZE 256
#define RENDER_SIZE 128

void renderPlayers(Client aClient, ClientView *pView) {
    static int frame = 0;
    frame++;
    int playerCount = NET_clientGetPlayerCount(aClient);

    for(int i = 0; i < playerCount; i++) {
        SDL_Point pos = NET_clientGetPlayerPos(aClient, i);

        int direction = NET_clientGetPlayerDirection(aClient, i);

        SDL_Rect playerRect = {pos.x, pos.y, RENDER_SIZE, RENDER_SIZE};
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
            pControl->isMouseDown,
            pControl->isMouseUp
        }
    };
    return pip;
}