#include "../../include/UI/panel.h"
#include <stdlib.h>
#include <string.h>
#include "../../include/UI/label.h"
#include "../../include/UI/button.h"

typedef struct component {
    void* pComp;
    char key[64];
    int type;
} Component;

typedef struct Panel {
    SDL_Rect rect;
    SDL_Color bg;
    SDL_Texture* pBgImage;
    bool hasImage;

    Component compList[20];
    int compCount;
} *Panel;

Panel UI_panelCreate() {
    Panel aPanel = malloc(sizeof(struct Panel));

    aPanel->compCount = 0;
    aPanel->bg = (SDL_Color){ .r = 0, .g = 0, .b = 0, .a = 0 };
    aPanel->rect = (SDL_Rect){ .x = 0, .y = 0, .w = 1, .h = 1 };
    aPanel->pBgImage = NULL;
    aPanel->hasImage = false;

    return aPanel;
}

void UI_panelAddComponent(Panel aPanel, void* comp, int type, char* key) {
    if (aPanel->compCount < 20) {
        aPanel->compList[aPanel->compCount].pComp = comp;
        aPanel->compList[aPanel->compCount].type = type;
        strcpy(aPanel->compList[aPanel->compCount].key, key);
        (aPanel->compCount)++;
    }
}

void UI_panelSetAppearance(Panel aPanel, SDL_Rect rect, SDL_Color src_bg) {
    aPanel->bg = src_bg;
    aPanel->rect = rect;
}

void UI_panelRender(SDL_Renderer* pRend, Panel aPanel) {
    if (aPanel->hasImage) {

    }
    else {
        SDL_SetRenderDrawColor(pRend, aPanel->bg.r, aPanel->bg.g, aPanel->bg.b, aPanel->bg.a);
        SDL_RenderFillRect(pRend, &aPanel->rect);
    }

    for (int i = 0; i < aPanel->compCount; i++) {
        switch (aPanel->compList[i].type) {
        case UI_LABEL:
            UI_labelRender(pRend, (Label)aPanel->compList[i].pComp);
            break;
        case UI_BUTTON:
            UI_buttonRenderer(pRend, (Button)aPanel->compList[i].pComp);
            break;
        }
    }
}