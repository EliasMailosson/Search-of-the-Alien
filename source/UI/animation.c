#include "../../include/UI//animation.h"
#include <stdlib.h>

typedef struct Animation {
    SDL_Rect rect;
    SDL_Texture **frames;
    int frameCols;
    int frameCount;
    int currentFrame;
    int frameW, frameH;

    int playbackMode;
} *Animation;

Animation UI_animationCreate() {
    Animation aAnimation = malloc(sizeof(struct Animation));

    aAnimation->currentFrame = 0;
    aAnimation->frameCount = 0;
    aAnimation->frameCols = 0;
    aAnimation->frameW = aAnimation->frameH = 0;
    aAnimation->frames = NULL;
    aAnimation->rect = (SDL_Rect){0,0,1,1};
    aAnimation->playbackMode = PLAYBACK_FORWARD;

    return aAnimation;
}

void UI_animationRender(SDL_Renderer *pRend, Animation aAnimation) {
    SDL_RenderCopy(pRend, aAnimation->frames[aAnimation->currentFrame], NULL, &aAnimation->rect);
}

int UI_animationGetMode(Animation aAnimation) {
    return aAnimation->playbackMode;
}

void UI_animationNextFrame(Animation aAnimation) {
    (aAnimation->currentFrame)++;
    aAnimation->currentFrame %= (aAnimation->frameCount);
}

void UI_animationPrevFrame(Animation aAnimation) {
    (aAnimation->currentFrame)--;
    aAnimation->currentFrame %= (aAnimation->frameCount);
}

void UI_animationSetMode(Animation aAnimation, int newMode) {
    aAnimation->playbackMode = newMode;
}

void UI_animationLoad(Animation aAnimation, SDL_Renderer *pRend, char *imgPath, int frameW, int frameH, int frameCount) {
    aAnimation->frameCount = frameCount;
    aAnimation->frameW = frameW;
    aAnimation->frameH = frameH;

    SDL_Surface *surface = IMG_Load(imgPath);
    if(surface == NULL) {
        printf("Could not load animation\n");
    }
    
    aAnimation->frameCols = surface->w / frameW;
    aAnimation->frames = malloc(sizeof(SDL_Texture*) * aAnimation->frameCount);

    for(int i = 0; i < frameCount; i++) {
        int x = (i % aAnimation->frameCols) * frameW;
        int y = (i / aAnimation->frameCols) * frameH;
        
        SDL_Surface* frameSurface = SDL_CreateRGBSurfaceWithFormat(
            0, 
            frameW, 
            frameH, 
            surface->format->BitsPerPixel, 
            surface->format->format
        );
        
        SDL_Rect srcRect = {x, y, frameW, frameH};
        SDL_BlitSurface(surface, &srcRect, frameSurface, NULL);
        
        aAnimation->frames[i] = SDL_CreateTextureFromSurface(pRend, frameSurface);
        SDL_FreeSurface(frameSurface);
        
        if(!aAnimation->frames[i]) {
            printf("Error creating texture for frame %d: %s\n", i, SDL_GetError());
        }
    }

    SDL_FreeSurface(surface);
}

void UI_animationSetDestination(Animation aAnimation, SDL_Rect rect) {
    aAnimation->rect = rect;
}

void UI_animationDestroy(Animation aAnimation) {
    if(aAnimation == NULL) {
        return;
    }

    for(int i = 0; i < aAnimation->frameCount; i++) {
        if(aAnimation->frames[i] != NULL) {
            SDL_DestroyTexture(aAnimation->frames[i]);
            aAnimation->frames[i] = NULL;
        }
    }
    
    if(aAnimation->frames != NULL) {
        free(aAnimation->frames);
        aAnimation->frames = NULL;
    }

    if(aAnimation != NULL) {
        free(aAnimation);
        aAnimation = NULL;
    }
}