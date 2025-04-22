#include "../include/hud.h"

struct Arrow{
    SDL_Texture *pImg;
    SDL_Rect pos;
};


struct Hud{
    Arrow indicators[MAX_CLIENTS-1];
    //
};


Arrow arrowCreate(int index, SDL_Renderer *pRend,SDL_Window *pWindow){
    Arrow aArrow = malloc(sizeof(struct Arrow));
    if(aArrow == NULL){
        fprintf(stderr,"Error allocating memory for Arrow\n");
        return NULL;
    }
    SDL_Surface *tmpSurface = IMG_Load(FILE_PHAT_ARROW);
    if(!tmpSurface){
        fprintf(stderr,"Error creating Surface for arrow, %s\n",IMG_GetError());
        free(aArrow);
        return NULL;
    }
    aArrow->pImg = SDL_CreateTextureFromSurface(pRend,tmpSurface);
    if(!aArrow->pImg){
        fprintf(stderr,"Error creating textur for arrow %d: %s",index,IMG_GetError());
        SDL_FreeSurface(tmpSurface);
        free(aArrow);
        return NULL;
    }
    SDL_FreeSurface(tmpSurface);
    SDL_Color tmpColor = {0};
    switch (index){
    case 1:tmpColor = SDL_RED;break;
    case 2:tmpColor = SDL_GREEN;break;
    case 3:tmpColor = SDL_CYAN;break;
    case 4:tmpColor = SDL_YELLOW;break;
    case 5:tmpColor = SDL_BLACK;break;
    case 6:tmpColor = SDL_BLUE;break;
    case 7:tmpColor = SDL_RED;break;
    case 8:tmpColor = SDL_RED;break;
    default:
        tmpColor = SDL_TRANSPARENT;
        break;
    }
    SDL_SetTextureColorMod(aArrow->pImg,tmpColor.r,tmpColor.g,tmpColor.b);
    // fixa storlek och pos
    return aArrow;
}   

Hud hudCreate(SDL_Renderer *pRend,SDL_Window *pWin){
    Hud aHud = malloc(sizeof(struct Hud));
    if(aHud == NULL){
        fprintf(stderr,"Error allocating memory for the Hud\n");
        return NULL;
    }
    for (int i = 0; i < MAX_CLIENTS-1; i++){
        aHud->indicators[i] = arrowCreate(i,pRend,pWin);
    }
    return aHud;
}

Hud hudDestroy(Hud aHud){
    for (int i = 0; i < MAX_CLIENTS-1; i++){
        if(aHud->indicators[i] != NULL) free(aHud->indicators[i]);
        aHud->indicators[i] = NULL;
    }
    if(aHud != NULL) free(aHud);
    aHud = NULL;
}

void hudRender(Hud aHud){
    
}
