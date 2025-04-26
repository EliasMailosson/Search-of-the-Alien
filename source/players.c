#include <stdio.h>
#include<string.h>
#include "../include/players.h"
#include "../include/NET/client.h"
#include "../include/game.h"
#include "../include/clientLife.h"

#define SPRITE_SIZE 256
#define RENDER_SIZE 128

void sortByYaxis(Client aClient, int playerCount, int indices[]){
    for (int i = 0; i < playerCount - 1; i++) {
        for (int j = 0; j < playerCount - i - 1; j++) {
            int ia = indices[j];
            int ib = indices[j + 1];

            SDL_Point pa = NET_clientGetPlayerPos(aClient, ia);
            SDL_Point pb = NET_clientGetPlayerPos(aClient, ib);

            if (pa.y > pb.y) {  // ascending sort → lowest y LAST
                int temp = indices[j];
                indices[j] = indices[j + 1];
                indices[j + 1] = temp;
            }
        }
    }
}

void renderPlayers(Client aClient, ClientView *pView) {
    static int frame = 0;
    frame++;
    int playerCount = NET_clientGetPlayerCount(aClient);

    int selfIndex = NET_clientGetSelfIndex(aClient);
    SDL_Point selfPos = NET_clientGetPlayerPos(aClient, selfIndex);
    int centerX = pView->windowWidth/2;
    int centerY = pView->windowHeight/2;
    int renderSizeHalf = pView->playerRenderSize/2;

    int sortedIndex[playerCount];

    for (int i = 0; i < playerCount; i++) {
        sortedIndex[i] = i;
    }

    sortByYaxis(aClient, playerCount, sortedIndex);

    for(int n = 0; n < playerCount; n++) {
        int i = sortedIndex[n];

        SDL_Point pos = NET_clientGetPlayerPos(aClient, i);
        int direction = NET_clientGetPlayerDirection(aClient, i);

        int worldOffsetX = pos.x - selfPos.x;
        int worldOffsetY = pos.y - selfPos.y;
        float scale = (float)pView->playerRenderSize / RENDER_SIZE;
        float screenOffsetX = worldOffsetX * scale;
        float screenOffsetY = worldOffsetY * scale;

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
                .x = (int)(centerX + screenOffsetX - renderSizeHalf),
                .y = (int)(centerY + screenOffsetY - renderSizeHalf),
                .w = pView->playerRenderSize,
                .h = pView->playerRenderSize
            };
        }
        pView->PlayerPos[i] = (SDL_Point){.x = playerRect.x, .y = playerRect.y};
        
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
        
        int playerCharacter = NET_clientGetPlayerCharacter(aClient, i);
        SDL_RenderCopy(pView->pRend, pView->playerTexture[playerCharacter], &src, &playerRect);
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
            pControl->keys[SDL_SCANCODE_E],
            pControl->keys[SDL_SCANCODE_SPACE],
            pControl->isMouseDown,
            // pControl->isMouseUp, en för mycket
        },
        .selecterPlayerCharacter = pControl->selectedCharacter
    };
    return pip;
}

void updatepos(){

}
void RenderPlayerName(Client aClient, ClientView *pView, int i, SDL_Rect playerRect){
    char username[MAX_USERNAME_LEN] = {0};
    username[MAX_USERNAME_LEN - 1] = '\0';
    NET_clientGetPlayerName(aClient, i, username);

    if (strlen(username) == 0 || !pView->fonts){
        printf("%s",username);
        printf("Font: %p", (void*)pView->fonts);
        return;
    }

    SDL_Color nameColor = NET_GetPlayerColor(aClient, i); // Server ska välja färgerna på namnen
    SDL_Color shadowColor = {0,0,0,255}; //skuggan 

    SDL_Surface* shadowSurface = TTF_RenderText_Blended(pView->fonts, username, shadowColor);
    SDL_Texture* shadowTexture = SDL_CreateTextureFromSurface(pView->pRend, shadowSurface);

    int nameWidth = shadowSurface->w;
    int nameHeight = shadowSurface->h;

    SDL_Rect nameRect = {
        .x = playerRect.x + (playerRect.w - nameWidth)/2,
        .y = playerRect.y - nameHeight - 5,
        .w = nameWidth,
        .h = nameHeight
    };

    SDL_Rect shadowRect = {
        .x = nameRect.x + 1,
        .y = nameRect.y + 1,
        .w = nameWidth,
        .h = nameHeight
    };

    SDL_RenderCopy(pView->pRend,shadowTexture,NULL,&shadowRect);
    SDL_FreeSurface(shadowSurface);
    SDL_DestroyTexture(shadowTexture);

    SDL_Surface* nameSurface = TTF_RenderText_Blended(pView->fonts, username, nameColor);
    SDL_Texture* nameTexture = SDL_CreateTextureFromSurface(pView->pRend, nameSurface);

    SDL_RenderCopy(pView->pRend, nameTexture, NULL, &nameRect);

    SDL_FreeSurface(nameSurface);
    SDL_DestroyTexture(nameTexture);
}