#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../include/players.h"
#include "../include/NET/client.h"
#include "../include/game.h"
#include "../include/clientLife.h"

// #define SPRITE_SIZE 256
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

void renderProjectiles(Client aClient, ClientView *pView) {
    Proj projList[MAX_CLIENT_PROJ];
    NET_clientGetProjList(aClient, projList);

    int centerX = pView->windowWidth/2;
    int centerY = pView->windowHeight/2;

    
    float scale = (float)pView->playerRenderSize / RENDER_SIZE;
    
    for(int i = 0; i < MAX_CLIENT_PROJ; i++) {
        if(projList[i].textureIdx != PROJ_TEX_NONE) {
            int screenX = (int)roundf(centerX - (float)projList[i].x * scale);
            int screenY = (int)roundf(centerY - (float)projList[i].y * scale);
            double angleDegrees = (projList[i].angle / 255.0) * 360;

            SDL_Rect projRect = (SDL_Rect){
                .x = screenX,
                .y = screenY,
                .w = 20,
                .h = 20
            };
            SDL_RenderCopyEx(pView->pRend, pView->projectileTexture[projList[i].textureIdx], NULL, &projRect, angleDegrees, NULL, SDL_FLIP_NONE);
            // SDL_SetRenderDrawColor(pView->pRend, 255, 255, 0, 255);
            // SDL_RenderFillRect(pView->pRend, &projRect);
        }
    }
}

void renderEnemy(Client aClient, ClientView *pView) {
    int graphicsMode = NET_clientGetGraphicsQuality(aClient);
    int SPRITE_SIZE = 256/pow(2, graphicsMode-1);
    static int frame = 0;
    frame++;
    int centerX = pView->windowWidth/2;
    int centerY = pView->windowHeight/2;

    int renderSizeHalf = pView->playerRenderSize/2;
    float scale = (float)pView->playerRenderSize / RENDER_SIZE;

    for(int i = 0; i < NET_clientGetEnemiesCount(aClient); i++) {
        SDL_Point pos = NET_clientGetEnemyPos(aClient, i);
        int screenX = (int)roundf(centerX - (float)pos.x * scale);
        int screenY = (int)roundf(centerY - (float)pos.y * scale);
        int direction = NET_clientGetEnemyDirection(aClient, i);
        if (direction < 0 || direction > 7) {
            fprintf(stderr, "ERROR: Invalid direction: %d\n", direction);
            direction = 0;  // or clamp, or skip rendering
        }

        SDL_Rect enemyRect;
        enemyRect = (SDL_Rect){
                .x = (int)(screenX - renderSizeHalf / 2),
                .y = (int)(screenY - renderSizeHalf / 2),
                .w = pView->playerRenderSize / 2,
                .h = pView->playerRenderSize / 2
            };
        SDL_Rect src;
        int graphicsModePow2 = (int)pow(2, graphicsMode);
        int currentFrame = ( (frame/graphicsModePow2) % (24/((int)pow(2, graphicsMode-1))) ) * (int)pow(2, graphicsMode-1);
        src = (SDL_Rect){currentFrame*SPRITE_SIZE, direction*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};

        //pView->PlayerPos[i] = (SDL_Point){.x = playerRect.x, .y = playerRect.y};
        // SDL_SetRenderDrawColor(pView->pRend, 255, 0, 0, 255);
        // SDL_RenderFillRect(pView->pRend, &enemyRect);

        SDL_RenderCopy(pView->pRend, pView->enemyTexture, &src, &enemyRect);
    }
}

void renderPlayers(Client aClient, ClientView *pView) {
    int graphicsMode = NET_clientGetGraphicsQuality(aClient);
    int SPRITE_SIZE = 256/pow(2, graphicsMode-1);
    static int frame = 0;
    frame++;
    int playerCount = NET_clientGetPlayerCount(aClient);
    static Uint32 damageTime = 0;

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

    // shadow
    for(int n = 0; n < playerCount; n++) {
        int i = sortedIndex[n];
        if(NET_clientGetState(aClient) != NET_clientGetClientState(aClient, i)) {
            continue;
        }

        SDL_Point pos = NET_clientGetPlayerPos(aClient, i);
        pos.x -= renderSizeHalf;
        pos.y -= pView->playerRenderSize;

        int worldOffsetX = pos.x - selfPos.x;
        int worldOffsetY = pos.y - selfPos.y;
        float scale = (float)pView->playerRenderSize / RENDER_SIZE;
        float screenOffsetX = worldOffsetX * scale;
        float screenOffsetY = worldOffsetY * scale;

        SDL_Rect playerRect;
        if(selfIndex == i) {
            playerRect = (SDL_Rect){
                .x = centerX - renderSizeHalf,
                .y = centerY + renderSizeHalf/6,
                .w = pView->playerRenderSize,
                .h = renderSizeHalf
            };
        }
        else {
            playerRect = (SDL_Rect){
                .x = (int)(centerX + screenOffsetX - pView->playerRenderSize/6),
                .y = (int)(centerY + screenOffsetY + pView->playerRenderSize/5) + renderSizeHalf + renderSizeHalf/6,
                .w = pView->playerRenderSize,
                .h = renderSizeHalf
            };
        }
        pView->PlayerPos[i] = (SDL_Point){.x = playerRect.x, .y = playerRect.y};
        
        SDL_RenderCopy(pView->pRend, pView->shadowTexture, NULL, &playerRect);

    }

    // character
    for(int n = 0; n < playerCount; n++) {
        int i = sortedIndex[n];
        if(NET_clientGetState(aClient) != NET_clientGetClientState(aClient, i)) {
            continue;
        }

        SDL_Point pos = NET_clientGetPlayerPos(aClient, i);
        pos.x -= renderSizeHalf;
        pos.y -= pView->playerRenderSize;
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
                .x = (int)(centerX + screenOffsetX - pView->playerRenderSize/6),
                .y = (int)(centerY + screenOffsetY + pView->playerRenderSize/5),
                .w = pView->playerRenderSize,
                .h = pView->playerRenderSize
            };
        }

        pView->PlayerPos[i] = (SDL_Point){.x = playerRect.x, .y = playerRect.y};

        int shootAnimationOffset = 0;
        if(NET_clientIsShooting(aClient, i)) shootAnimationOffset = 3;
        
        int graphicsModePow2 = (int)pow(2, graphicsMode);
        int currentPlayerFrame = ( (frame/graphicsModePow2) % (24/((int)pow(2, graphicsMode-1))) );

        SDL_Rect src;
        switch(NET_clientGetPlayerAnimation(aClient, i)) {
            case ANIMATION_IDLE:
                src = (SDL_Rect){currentPlayerFrame*SPRITE_SIZE, (direction+8)*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
                break;
            case ANIMATION_RUNNING:
                src = (SDL_Rect){currentPlayerFrame*SPRITE_SIZE, direction*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
                break;
            default:
                src = (SDL_Rect){currentPlayerFrame*SPRITE_SIZE, direction*SPRITE_SIZE, SPRITE_SIZE, SPRITE_SIZE};
        }
        
        int playerCharacter = NET_clientGetPlayerCharacter(aClient, i) + shootAnimationOffset;
        SDL_RenderCopy(pView->pRend, pView->playerTexture[playerCharacter], &src, &playerRect);
        RenderPlayerName(aClient, pView, i, playerRect);

        //printf("Players HP: %d\n", (int)NET_clientGetHP(aClient, i));
        /*if(NET_clientGetHP(aClient, i) < 100){ 
            SDL_SetRenderDrawColor(pView->pRend, 255, 0, 0, 255);
            SDL_RenderFillRect(pView->pRend, &playerRect);
        }*/

        SDL_Rect vignetteRect = {.x = 0, .y = 0, .w = 1920, .h = 1080};
        SDL_Rect screenRect = {.x = 0, .y = 0, .w = pView->windowWidth, .h = pView->windowHeight};

        if (NET_clientIsPlayerDamaged(aClient, selfIndex)) {
            damageTime = SDL_GetTicks();
        }
        if (SDL_GetTicks() - damageTime < 250) {
            SDL_RenderCopy(pView->pRend, pView->vignetteTexture, &vignetteRect, &screenRect);
        }
        // SDL_Rect rpoint = {centerX-5,centerY-5 + renderSizeHalf, 10, 10};
    }
}

PlayerInputPacket prepareInputArray(ClientControl *pControl, int windowWidth, int windowHeight) {
    static bool mouseDown = false;
    if(pControl->isMouseDown) {
        mouseDown = true;
    } else if(pControl->isMouseUp) {
        mouseDown = false;
    }
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
            mouseDown,
            // pControl->isMouseUp,
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