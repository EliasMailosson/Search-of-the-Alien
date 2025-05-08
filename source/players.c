#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "../include/players.h"
#include "../include/NET/client.h"
#include "../include/game.h"
#include "../include/clientLife.h"

#define MAX_ENTITIES (MAX_CLIENTS + MAX_ENEMIES_CLIENT_SIDE)
#define RENDER_SIZE 128

enum EntityType{ENTITY_ENEMY, ENTITY_PLAYER};

typedef struct entity{
    void *instance;
    int type;
    int screenX, screenY;
}Entity;
typedef struct entityContext {
    Entity currentEntity;
    int graphicsMode;
    int spriteSize;
    int frame;
    int selfIndex;
    SDL_Point selfPos;
    int centerX;
    int centerY;
    int renderSizeHalf;
    float scale;
}EntityContext;

int cmpYPos(void* a, void* b) {
    Entity *ea = (Entity*)a;
    Entity *eb = (Entity*)b;
    int aY = ea->screenY;
    int bY = eb->screenY;

    if (aY < bY) return -1;
    else if (aY > bY) return 1;
    else return 0;
}

void sortEntities(Client aClient, int *count, Entity *outputEntityList, EntityContext context, ClientView *pView) {
    int n = 0;
    for(int i = 0; i < NET_clientGetPlayerCount(aClient); i++) {
        if(NET_clientGetState(aClient) != NET_clientGetClientState(aClient, i)) {
            continue;
        }
        outputEntityList[n].instance = NET_clientGetPlayer(aClient, i);
        outputEntityList[n].type = ENTITY_PLAYER;

        Player player = *((Player*)outputEntityList[n].instance);
        player.pos.x -= context.renderSizeHalf;
        player.pos.y -= pView->playerRenderSize;

        int worldOffsetX = player.pos.x - context.selfPos.x;
        int worldOffsetY = player.pos.y - context.selfPos.y;
        float screenOffsetX = worldOffsetX * context.scale;
        float screenOffsetY = worldOffsetY * context.scale;

        if(NET_clientGetSelfIndex(aClient) == i) {
            outputEntityList[n].screenX = context.centerX - context.renderSizeHalf;
            outputEntityList[n].screenY = context.centerY - context.renderSizeHalf;
        }
        else {
            outputEntityList[n].screenX = (int)(context.centerX + screenOffsetX - pView->playerRenderSize/6);
            outputEntityList[n].screenY = (int)(context.centerY + screenOffsetY + pView->playerRenderSize/5);
        }
        pView->PlayerPos[i] = (SDL_Point){.x = outputEntityList[n].screenX, .y = outputEntityList[n].screenY};

        n++;
    }
    for(int i = 0; i < NET_clientGetEnemiesCount(aClient); i++) {
        outputEntityList[n].instance = NET_clientGetEnemy(aClient, i);
        outputEntityList[n].type = ENTITY_ENEMY;

        ClientEnemy enemy = *((ClientEnemy*)outputEntityList[n].instance);
        int screenX = (int)roundf(context.centerX - (float)enemy.pos.x * context.scale);
        int screenY = (int)roundf(context.centerY - (float)enemy.pos.y * context.scale);
        outputEntityList[n].screenX = (int)(screenX - context.renderSizeHalf / 2);
        outputEntityList[n].screenY = (int)(screenY - context.renderSizeHalf / 2);

        n++;
    }
    *count = n;

    qsort(outputEntityList, n, sizeof(Entity), cmpYPos);
}

void renderPlayer(EntityContext context, ClientView *pView) {
    Player player = *((Player*)context.currentEntity.instance);
    SDL_Rect playerRect = {
        .x = context.currentEntity.screenX,
        .y = context.currentEntity.screenY,
        .w = pView->playerRenderSize,
        .h = pView->playerRenderSize
    };

    int shootAnimationOffset = 0;
    if(player.isShooting) shootAnimationOffset = 3;
    
    int graphicsModePow2 = (int)pow(2, context.graphicsMode);
    int currentPlayerFrame = ( (context.frame/graphicsModePow2) % (24/((int)pow(2, context.graphicsMode-1))) );

    SDL_Rect src;
    switch(player.currentPlayerAnimation) {
        case ANIMATION_IDLE:
            src = (SDL_Rect){currentPlayerFrame*context.spriteSize, (player.direction+8)*context.spriteSize, context.spriteSize, context.spriteSize};
            break;
        case ANIMATION_RUNNING:
            src = (SDL_Rect){currentPlayerFrame*context.spriteSize, player.direction*context.spriteSize, context.spriteSize, context.spriteSize};
            break;
        default:
            src = (SDL_Rect){currentPlayerFrame*context.spriteSize, player.direction*context.spriteSize, context.spriteSize, context.spriteSize};
    }
    
    int playerCharacter = player.playerCharacter + shootAnimationOffset;
    SDL_RenderCopy(pView->pRend, pView->playerTexture[playerCharacter], &src, &playerRect);
    RenderPlayerName(pView, player, playerRect);
}

void renderEnemy(EntityContext context, ClientView *pView) {
    ClientEnemy enemy = *((ClientEnemy*)context.currentEntity.instance);

    if (enemy.direction < 0 || enemy.direction > 7) {
        fprintf(stderr, "ERROR: Invalid direction: %d\n", enemy.direction);
        enemy.direction = 0;
    }

    SDL_Rect enemyRect;
    enemyRect = (SDL_Rect){
            .x = context.currentEntity.screenX,
            .y = context.currentEntity.screenY,
            .w = pView->playerRenderSize / 2,
            .h = pView->playerRenderSize / 2
    };
    SDL_Rect src;

    int graphicsModePow2 = (int)pow(2, context.graphicsMode);
    int currentFrame = ( (context.frame/graphicsModePow2) % (24/((int)pow(2, context.graphicsMode-1))) );
    src = (SDL_Rect){currentFrame*context.spriteSize, enemy.direction*context.spriteSize, context.spriteSize, context.spriteSize};

    SDL_RenderCopy(pView->pRend, pView->enemyTexture, &src, &enemyRect);
}

void renderShadows(EntityContext context, Entity *entityList, ClientView *pView, int count) {
    for(int i = 0; i < count; i++) {
        context.currentEntity = entityList[i];
        SDL_Rect rect;
        if(context.currentEntity.type == ENTITY_PLAYER) {
            rect = (SDL_Rect){
                .x = context.currentEntity.screenX,
                .y = context.currentEntity.screenY + context.renderSizeHalf + context.renderSizeHalf/6,
                .w = pView->playerRenderSize,
                .h = context.renderSizeHalf
            };
        }
        else if(context.currentEntity.type == ENTITY_ENEMY) {
            rect = (SDL_Rect){
                .x = context.currentEntity.screenX,
                .y = context.currentEntity.screenY + context.renderSizeHalf*2/3,
                .w = pView->playerRenderSize/2,
                .h = context.renderSizeHalf/2
            };
        }
        
        SDL_RenderCopy(pView->pRend, pView->shadowTexture, NULL, &rect);
    }
}

void renderEntities(Client aClient, ClientView *pView) {
    Entity entityList[MAX_ENTITIES];
    int count = 0;

    EntityContext context;
    context.graphicsMode = NET_clientGetGraphicsQuality(aClient);
    context.spriteSize = 256/pow(2, context.graphicsMode-1);
    static int frame = 0; frame++;
    context.frame = frame;
    int playerCount = NET_clientGetPlayerCount(aClient);
    static Uint32 damageTime = 0;
    context.selfIndex = NET_clientGetSelfIndex(aClient);
    context.selfPos = NET_clientGetPlayerPos(aClient, context.selfIndex);
    context.centerX = pView->windowWidth/2;
    context.centerY = pView->windowHeight/2;
    context.renderSizeHalf = pView->playerRenderSize/2;
    context.scale = (float)pView->playerRenderSize / RENDER_SIZE;

    sortEntities(aClient, &count, entityList, context, pView);
    renderShadows(context, entityList, pView, count);
    for(int i = 0; i < count; i++) {
        context.currentEntity = entityList[i];
        switch(entityList[i].type) {
            case ENTITY_ENEMY:
                renderEnemy(context, pView);
                break;

            case ENTITY_PLAYER:
                renderPlayer(context, pView);
                break;
        }
    }

    SDL_Rect vignetteRect = {.x = 0, .y = 0, .w = 1920, .h = 1080};
    SDL_Rect screenRect = {.x = 0, .y = 0, .w = pView->windowWidth, .h = pView->windowHeight};
    if (NET_clientIsPlayerDamaged(aClient, context.selfIndex)) {
        damageTime = SDL_GetTicks();
    }
    if (SDL_GetTicks() - damageTime < 250) {
        SDL_RenderCopy(pView->pRend, pView->vignetteTexture, &vignetteRect, &screenRect);
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
void RenderPlayerName(ClientView *pView, Player player, SDL_Rect playerRect){
    if (strlen(player.username) == 0 || !pView->fonts){
        printf("%s",player.username);
        printf("Font: %p", (void*)pView->fonts);
        return;
    }

    SDL_Color nameColor = player.color;
    SDL_Color shadowColor = {0,0,0,255};

    SDL_Surface* shadowSurface = TTF_RenderText_Blended(pView->fonts, player.username, shadowColor);
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

    SDL_Surface* nameSurface = TTF_RenderText_Blended(pView->fonts, player.username, nameColor);
    SDL_Texture* nameTexture = SDL_CreateTextureFromSurface(pView->pRend, nameSurface);

    SDL_RenderCopy(pView->pRend, nameTexture, NULL, &nameRect);

    SDL_FreeSurface(nameSurface);
    SDL_DestroyTexture(nameTexture);
}

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