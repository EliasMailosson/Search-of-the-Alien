#include "../../include/UI/panel.h"
#include <stdlib.h>

typedef struct component {
    void *pComp;
    char *key;
} Component;

typedef struct Panel {
    SDL_Rect rect;
    SDL_Color bg;
    SDL_Texture *pBgImage;
    bool hasImage;

    Component *compList;
    int compCount;
} *Panel;

Panel UI_panelCreate() {
    Panel aPanel = malloc(sizeof(struct Panel));

    aPanel->compList = NULL;
    aPanel->compCount = 0;
    aPanel->bg = (SDL_Color){.r=0, .g=0, .b=0, .a=0};
    aPanel->rect = (SDL_Rect){.x=0, .y=0, .w=1, .h=1};
    aPanel->pBgImage = NULL;
    aPanel->hasImage = false;

    return aPanel;
}

void UI_panelSetAppearance(Panel aPanel, SDL_Rect rect, SDL_Color src_bg) {
    aPanel->bg = src_bg;
    aPanel->rect = rect;
}

void UI_panelRender(SDL_Renderer *pRend, Panel aPanel) {
    if(aPanel->hasImage) {

    }
    else {
        SDL_SetRenderDrawColor(pRend, aPanel->bg.r, aPanel->bg.g, aPanel->bg.b, aPanel->bg.a);
        SDL_RenderFillRect(pRend, &aPanel->rect);
    }
}