#include "../../include/UI/label.h"
#include <stdlib.h>
#include <string.h>

typedef struct Label {
    int x, y;
    char text[256];
    SDL_Color color;
    TTF_Font* pFont;
    SDL_Texture* pTexture;
    SDL_Rect textRect;
} *Label;

Label UI_labelCreate() {
    Label aLabel = malloc(sizeof(struct Label));

    aLabel->pTexture = NULL;
    aLabel->pFont = NULL;
    aLabel->color = (SDL_Color){ .r = 255, .g = 255, .b = 255, .a = 255 };
    aLabel->textRect = (SDL_Rect){ .x = 0, .y = 0, .w = 1, .h = 1 };
    aLabel->x = 0;
    aLabel->y = 0;
    strcpy(aLabel->text, "a label");

    return aLabel;
}

void UI_labelSetText(Label aLabel, char* text) {
    strcpy(aLabel->text, text);
}

void UI_labelSetPosition(Label aLabel, int x, int y) {
    aLabel->x = x;
    aLabel->y = y;
}

void UI_labelGetDimensions(Label aLabel, int* w, int* h) {
    *w = aLabel->textRect.w;
    *h = aLabel->textRect.h;
}

void UI_labelSetAppearance(SDL_Renderer* pRend, Label aLabel, int x, int y, SDL_Color color, TTF_Font* pFont) {
    aLabel->color = color;
    aLabel->x = x;
    aLabel->y = y;
    aLabel->pFont = pFont;

    SDL_Surface* surface = TTF_RenderText_Blended(aLabel->pFont, aLabel->text, aLabel->color);
    aLabel->pTexture = SDL_CreateTextureFromSurface(pRend, surface);
    aLabel->textRect = (SDL_Rect){ .x = aLabel->x, .y = aLabel->y, .w = surface->w, .h = surface->h };
    SDL_FreeSurface(surface);
}

void UI_labelRefreshTexture(SDL_Renderer* pRend, Label aLabel) {
    SDL_Surface* surface = TTF_RenderText_Blended(aLabel->pFont, aLabel->text, aLabel->color);
    aLabel->pTexture = SDL_CreateTextureFromSurface(pRend, surface);
    aLabel->textRect = (SDL_Rect){ .x = aLabel->x, .y = aLabel->y, .w = surface->w, .h = surface->h };
    SDL_FreeSurface(surface);
}

void UI_labelRender(SDL_Renderer* pRend, Label aLabel) {
    SDL_RenderCopy(pRend, aLabel->pTexture, NULL, &aLabel->textRect);
}

void UI_labelDestroy(Label aLabel) {
    SDL_DestroyTexture(aLabel->pTexture);
    aLabel->pTexture = NULL;
    free(aLabel);
}