#include "../../include/HUD/hud.h"
#include "../../include/UI/label.h"
#include <stdlib.h>
#include <string.h>

typedef struct playerList {
    int x;
    Label usernames[MAX_CLIENTS];
    TTF_Font *pFont;
    int count;
} PlayerList;

typedef struct Objectives {
    int x, y;
    Label objectiveText;
    TTF_Font *pFont;
} Objectives;

struct Arrow{
    double angel;
    SDL_FRect pos;
    bool isPlayerOnScreen;
};

struct Hud{
    SDL_Texture *imgArrow[MAX_CLIENTS]; 
    Arrow indicators[MAX_CLIENTS-1];
    PlayerList playerList;
    SDL_Texture *playerIconTexture[3];
    Objectives objectives[OBJECTIVECOUNT];
};

Arrow arrowCreate(){
    Arrow aArrow = malloc(sizeof(struct Arrow));
    if(aArrow == NULL){
        fprintf(stderr,"Error allocating memory for Arrow\n");
        return NULL;
    }
    aArrow->pos = (SDL_FRect){.h = ARROW_SIZE,.w = ARROW_SIZE, .x = 40, .y = 40};
    aArrow->angel = 0;
    aArrow->isPlayerOnScreen = true;
    return aArrow;
}   

Hud hudCreate(SDL_Renderer *pRend){
    Hud aHud = malloc(sizeof(struct Hud));
    if(aHud == NULL){
        fprintf(stderr,"Error allocating memory for the Hud\n");
        return NULL;
    }
    for (int i = 0; i < MAX_CLIENTS; i++){
        char *file;
        switch (i){
        case 0: file = FILE_PHAT_ARROW_RED; break;
        case 1: file = FILE_PHAT_ARROW_GREEN; break;
        case 2: file = FILE_PHAT_ARROW_BLUE; break;
        case 3: file = FILE_PHAT_ARROW_ORANGE; break;
        case 4: file = FILE_PHAT_ARROW_PURPLE; break;
        case 5: file = FILE_PHAT_ARROW_CYAN; break;
        case 6: file = FILE_PHAT_ARROW_MAGENTA; break;
        case 7: file = FILE_PHAT_ARROW_YELLO; break;
        default:
            file = FILE_PHAT_ARROW_BLACK;
            break;
        }
        SDL_Surface *tmpSurface = IMG_Load(file);
        if(!tmpSurface){
            fprintf(stderr,"Error creating Surface for arrow, %s\n",IMG_GetError());
            free(aHud);
            return NULL;
        }
        aHud->imgArrow[i] = SDL_CreateTextureFromSurface(pRend,tmpSurface);
        if(!aHud->imgArrow[i]){
            fprintf(stderr,"Error creating textur for arrow %d: %s",i,IMG_GetError());
            SDL_FreeSurface(tmpSurface);
            free(aHud);
            return NULL;
        }
        SDL_FreeSurface(tmpSurface);
    }
    for (int i = 0; i < MAX_CLIENTS-1; i++){
        aHud->indicators[i] = arrowCreate();
    }

    aHud->playerList.pFont = TTF_OpenFont("assets/fonts/jura.ttf", 20);
    for(int i = 0; i < MAX_CLIENTS; i++) {
        aHud->playerList.usernames[i] = UI_labelCreate();
    }

    //OBJECTIVES
    aHud->objectives->pFont = TTF_OpenFont("assets/fonts/jura.ttf", 20);
    for (int i = 0; i < OBJECTIVECOUNT; i++) {
       aHud->objectives[i].objectiveText = UI_labelCreate(); 
    }
    

    SDL_Surface *surf1 = IMG_Load("assets/images/hud/biggie-icon.png");
    aHud->playerIconTexture[1] = SDL_CreateTextureFromSurface(pRend, surf1);
    SDL_FreeSurface(surf1);
    SDL_Surface *surf2 = IMG_Load("assets/images/hud/blowface-icon.png");
    aHud->playerIconTexture[0] = SDL_CreateTextureFromSurface(pRend, surf2);
    SDL_FreeSurface(surf2);
    SDL_Surface *surf3 = IMG_Load("assets/images/hud/cleo-icon.png");
    aHud->playerIconTexture[2] = SDL_CreateTextureFromSurface(pRend, surf3);
    SDL_FreeSurface(surf3);
    return aHud;
}

void hudDestroy(Hud aHud){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if(aHud->imgArrow[i]) SDL_DestroyTexture(aHud->imgArrow[i]);
    }
    for (int i = 0; i < MAX_CLIENTS-1; i++){
        if(aHud->indicators[i] != NULL) free(aHud->indicators[i]);
        aHud->indicators[i] = NULL;
    }
    for(int i = 0; i < 3; i++) {
        if(aHud->playerIconTexture[i] != NULL) {
            SDL_DestroyTexture(aHud->playerIconTexture[i]);
        }
    }
    
    aHud->objectives->pFont = TTF_OpenFont("assets/fonts/jura.ttf", 20);
    for (int i = 0; i < OBJECTIVECOUNT; i++) {
       aHud->objectives[i].objectiveText = UI_labelCreate(); 
    }
    TTF_CloseFont(aHud->objectives->pFont);
    TTF_CloseFont(aHud->playerList.pFont);
    aHud->playerList.pFont = NULL;

    if(aHud != NULL) free(aHud);
    aHud = NULL;
}

static void arrowRender(Arrow aArrow,SDL_Renderer *pRend,SDL_Texture *pImg){
    if(!aArrow->isPlayerOnScreen){
        if(SDL_RenderCopyExF(pRend,pImg,NULL,&aArrow->pos,aArrow->angel,NULL,SDL_FLIP_NONE) == -1){
            printf("filde to render\n");
        }
    }
}

void updateHudPlayerList(Client aClient, Hud aHud, SDL_Renderer *pRend, int windowW, int windowH) {
    aHud->playerList.count = NET_clientGetPlayerCount(aClient);
    int sortArr[MAX_CLIENTS];
    int newCount = 0;
    for(int i = 0; i < aHud->playerList.count; i++) {
        if(NET_clientGetState(aClient) == NET_clientGetClientState(aClient, i)) {
            sortArr[newCount++] = i;
        }
    }
    aHud->playerList.x = windowW - 140;
    for(int n = 0; n < newCount; n++) {
        int i = sortArr[n];
        SDL_Color color = NET_GetPlayerColor(aClient, i);
        UI_labelSetAppearance(pRend, aHud->playerList.usernames[i], aHud->playerList.x, 10 + i*40, color, aHud->playerList.pFont);

        char username[48];
        NET_clientGetPlayerName(aClient, i, username);
        UI_labelSetText(aHud->playerList.usernames[i], username);
        UI_labelRefreshTexture(pRend, aHud->playerList.usernames[i]);
    }
    aHud->playerList.count = newCount;
}

void hudRender(Client aClient, Hud aHud,SDL_Renderer *pRend, int windowW, int windowH){
    for (int i = 0; i < NET_clientGetPlayerCount(aClient); i++){
        int colorIdx = NET_clientGetPlayerColorIndex(aClient,i);
        arrowRender(aHud->indicators[i],pRend,aHud->imgArrow[colorIdx]);
    }

    for(int i = 0; i < aHud->playerList.count; i++) {
        SDL_Rect r = {
            .x = aHud->playerList.x - 34,
            .y = 14 + i*40,
            .w = 28,
            .h = 28
        };
        int character = NET_clientGetPlayerCharacter(aClient, i);
        SDL_RenderCopy(pRend, aHud->playerIconTexture[character], NULL, &r);
        UI_labelRender(pRend, aHud->playerList.usernames[i]);

        SDL_SetRenderDrawColor(pRend, 255, 255, 255, 255);
        SDL_RenderFillRect(pRend, &((SDL_Rect){aHud->playerList.x, r.y + 24, 100, 4}));
    }

    int dashCooldown = NET_clientGetDashCooldown(aClient);
    int centerX = windowW/2;
    int centerY = windowH/2;
    if(dashCooldown != 100) {
        SDL_SetRenderDrawColor(pRend, 150, 150, 150, 180);
        SDL_RenderFillRect(pRend, &((SDL_Rect){centerX - 10, centerY+20, 20, 2}));
        SDL_SetRenderDrawColor(pRend, 255, 255, 255, 180);
        SDL_RenderFillRect(pRend, &((SDL_Rect){centerX - 10, centerY+20, dashCooldown/5, 2}));
    }
    
}

SDL_Point hudGettArrowPos(Hud aHud, int index){
    return (SDL_Point){
        .x = aHud->indicators[index]->pos.x,
        .y = aHud->indicators[index]->pos.y
    };
}

void updateArrows(Hud aHud,SDL_Window *pWin,Client aClient, SDL_Point playerPos[MAX_CLIENTS]){
    int screenHeight = 0, screenWidth = 0;
    SDL_GetWindowSize(pWin,&screenWidth,&screenHeight);
    SDL_Point center = (SDL_Point){.x = screenWidth/2,.y = screenHeight/2};
    SDL_Rect screen = {.h = screenHeight, .w = screenWidth, .x = 0,.y = 0};
    int selfIndex = NET_clientGetSelfIndex(aClient);
    double radians ={0};
    float dx = {0}, dy = {0};
    float tx = {0}, ty = {0}, t = {0}, x = 0, y = 0;
    for (int i = 0; i < NET_clientGetPlayerCount(aClient); i++){
        if(selfIndex == i) continue;
        if(pointInRect(screen,playerPos[i])){
            aHud->indicators[i]->isPlayerOnScreen = true;
            continue;
        }else{
            aHud->indicators[i]->isPlayerOnScreen = false;
        }
        dx = (playerPos[i].x - center.x);
        dy = (playerPos[i].y - center.y);
        radians = atan2(-(double)dx,-(double)dy);
        aHud->indicators[i]->angel = -((double)radians* 180.0 / M_PI);
        radians = atan2((double)dy,(double)dx);
        //position
        float len = hypotf(dx, dy);
        dx = (dx/len);
        dy = (dy/len);

        tx = (dx != 0.0f) ? center.x / fabsf(dx) : INFINITY;
        ty = (dy != 0.0f) ? center.y / fabsf(dy) : INFINITY;
        t = (tx < ty) ? tx : ty;

        x = (center.x + dx * t) - ARROW_SIZE;
        y = (center.y + dy * t) - ARROW_SIZE;
        
        x = fmaxf(0.0f, fminf(x, screenWidth  - ARROW_SIZE));
        y = fmaxf(0.0f, fminf(y, screenHeight - ARROW_SIZE));

        aHud->indicators[i]->pos.x = x;
        aHud->indicators[i]->pos.y = y;
        
        //printf("x: %f, y: %f\n",aHud->indicators[i]->pos.x,aHud->indicators[i]->pos.y);
    }
}

bool pointInRect(SDL_Rect rect, SDL_Point point){
    if (point.x >= rect.x && 
        point.x <  rect.x + rect.w && 
        point.y >= rect.y && 
        point.y <  rect.y + rect.h){
        return true;
    }
    return false;
}