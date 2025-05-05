#include "../../include/HUD/hud.h"

struct Arrow{
    double angel;
    SDL_FRect pos;
    bool isPlayerOnScreen;
};

struct Hud{
    SDL_Texture *imgArrow[MAX_CLIENTS]; 
    Arrow indicators[MAX_CLIENTS-1];
    //
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

void hudRender(Hud aHud,SDL_Renderer *pRend,int colerIndex,int i){
    arrowRender(aHud->indicators[i],pRend,aHud->imgArrow[colerIndex]);
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