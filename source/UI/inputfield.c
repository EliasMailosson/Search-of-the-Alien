#include "../../include/UI/inputfield.h"
#include <stdlib.h>
#include <string.h>

typedef struct Inputfield {
    SDL_Rect boxRect, textRect;
    SDL_Color bg, fg;
    TTF_Font* pFont;
    SDL_Texture* pTexture;

    bool isFocused;
    char buffer[256];
    int bufferLenght;
    int cursorIdx;
    int cursorX;
} *Inputfield;

Inputfield UI_inputfieldCreate() {
    Inputfield aInputfield = malloc(sizeof(struct Inputfield));

    aInputfield->boxRect = (SDL_Rect){.x=0, .y=0, .w=1, .h=1};
    aInputfield->textRect = (SDL_Rect){.x=0, .y=0, .w=1, .h=1};

    strcpy(aInputfield->buffer, " ");
    aInputfield->bufferLenght = 1;
    aInputfield->cursorIdx = 0;
    aInputfield->cursorX = 0;
    aInputfield->pFont = NULL;
    aInputfield->bg = (SDL_Color){.r=0, .g=0, .b=0, .a=255};
    aInputfield->fg = (SDL_Color){.r=255, .g=255, .b=255, .a=255};
    aInputfield->pTexture = NULL;
    aInputfield->isFocused = false;

    return aInputfield;
}

void UI_inputfieldSetFocus(Inputfield aInputfield, int mouseX, int mouseY) {
    bool mouseOverField = (
        (mouseX >= aInputfield->boxRect.x) &&
        (mouseX <= (aInputfield->boxRect.x + aInputfield->boxRect.w)) &&
        (mouseY >= aInputfield->boxRect.y) &&
        (mouseY <= (aInputfield->boxRect.y + aInputfield->boxRect.h))
    );

    aInputfield->isFocused = mouseOverField;

    if(aInputfield->isFocused) {
        SDL_StartTextInput();
    } 
    else SDL_StopTextInput();
}

void UI_inputfieldUpdateBuffer(Inputfield aInputfield, char *text) {
    char temp[256];
    char end[256];

    memcpy(temp, aInputfield->buffer, aInputfield->cursorIdx);
    temp[aInputfield->cursorIdx] = '\0';

    memcpy(end, aInputfield->buffer + aInputfield->cursorIdx, aInputfield->bufferLenght - aInputfield->cursorIdx);
    end[aInputfield->bufferLenght - aInputfield->cursorIdx] = '\0';

    strcat(temp, text);
    strcat(temp, end);

    strcpy(aInputfield->buffer, temp);

    aInputfield->bufferLenght = strlen(aInputfield->buffer);
    aInputfield->cursorIdx += strlen(text);
}

void UI_inputfieldDestroy(Inputfield aInputfield) {
    SDL_DestroyTexture(aInputfield->pTexture);
    free(aInputfield);

    if(SDL_IsTextInputActive()) {
        SDL_StopTextInput();
    }
}

bool UI_inputfieldIsFocused(Inputfield aInputfield) {
    return aInputfield->isFocused;
}

void UI_inputfieldRender(SDL_Renderer* pRend, Inputfield aInputfield) {
    static int cursorBlink = 0;

    SDL_SetRenderDrawColor(pRend, aInputfield->bg.r, aInputfield->bg.g, aInputfield->bg.b, aInputfield->bg.a);
    SDL_RenderFillRect(pRend, &aInputfield->boxRect);

    if(aInputfield->bufferLenght > 0) {
        SDL_RenderCopy(pRend, aInputfield->pTexture, NULL, &aInputfield->textRect);
    }
    
    if(cursorBlink++ % 36 > 12 && aInputfield->isFocused) {
        int cursorWidth = 2;
        SDL_SetRenderDrawColor(pRend, aInputfield->fg.r, aInputfield->fg.g, aInputfield->fg.b, aInputfield->fg.a);
        SDL_Rect cursor = {aInputfield->textRect.x + aInputfield->cursorX, aInputfield->textRect.y, cursorWidth, aInputfield->textRect.h};
        SDL_RenderFillRect(pRend, &cursor);
    }
}

void UI_inputfieldBackspace(Inputfield aInputfield) {
    static int delay = 0;
    delay++;

    if(delay > 3) {
        if(aInputfield->bufferLenght <= 0 || aInputfield->cursorIdx <= 0) {
            return;
        }
        char temp[256];
        char end[256];

        memcpy(temp, aInputfield->buffer, aInputfield->cursorIdx - 1);
        temp[aInputfield->cursorIdx - 1] = '\0';

        memcpy(end, aInputfield->buffer + aInputfield->cursorIdx, aInputfield->bufferLenght - aInputfield->cursorIdx);
        end[aInputfield->bufferLenght - aInputfield->cursorIdx] = '\0';

        strcat(temp, end);

        strcpy(aInputfield->buffer, temp);

        aInputfield->bufferLenght = strlen(aInputfield->buffer);
        (aInputfield->cursorIdx)--;
        if(aInputfield->cursorIdx < 0) aInputfield->cursorIdx = 0;

        delay = 0;
    }
}

void UI_inputfieldGetBuffer(Inputfield aInputfield, char *dest) {
    strcpy(dest, aInputfield->buffer);
}

void UI_inputfieldResetBuffer(Inputfield aInputfield) {
    strcpy(aInputfield->buffer, " ");
    aInputfield->bufferLenght = 1;
    aInputfield->cursorIdx = 0;
    aInputfield->cursorX = 0;
}

void UI_inputfieldRefreshTexture(SDL_Renderer* pRend, Inputfield aInputfield) {
    if(aInputfield->bufferLenght <= 0) {
        aInputfield->cursorX = 0;
        return;
    }
    SDL_Surface *surface = TTF_RenderText_Blended(aInputfield->pFont, aInputfield->buffer, aInputfield->fg);
    aInputfield->pTexture = SDL_CreateTextureFromSurface(pRend, surface);

    aInputfield->boxRect.h = surface->h + 8;
    aInputfield->textRect.x = aInputfield->boxRect.x + 4;
    aInputfield->textRect.y = aInputfield->boxRect.y + 4;

    if (surface->w > aInputfield->boxRect.w - 8) {
        
    }

    aInputfield->textRect.w = surface->w;
    aInputfield->textRect.h = surface->h;
    SDL_FreeSurface(surface);

    if(aInputfield->cursorIdx <= 0) {
        aInputfield->cursorX = 0;
    }
    else {
        char temp[256];
        memcpy(temp, aInputfield->buffer, aInputfield->cursorIdx);
        temp[aInputfield->cursorIdx] = '\0';
        surface = TTF_RenderText_Blended(aInputfield->pFont, temp, aInputfield->fg);
        aInputfield->cursorX = surface->w;
        SDL_FreeSurface(surface);
    }
}

void UI_inputfieldMoveCursor(Inputfield aInputfield, bool direction) {
    static int delay = 0;
    delay++;

    if(delay > 4) {
        if(direction) {
            (aInputfield->cursorIdx)++;
        } 
        else if(!direction) {
            (aInputfield->cursorIdx)--;
        } 

        if(aInputfield->cursorIdx < 0) aInputfield->cursorIdx = 0;
        if(aInputfield->cursorIdx >= aInputfield->bufferLenght) aInputfield->cursorIdx = aInputfield->bufferLenght;

        delay = 0;
    }
}

void UI_inputfieldSetAppearance(SDL_Renderer* pRend, Inputfield aInputfield, int x, int y, int w, SDL_Color bg, SDL_Color fg, TTF_Font* pFont) {
    aInputfield->boxRect.x = x;
    aInputfield->boxRect.y = y;
    aInputfield->boxRect.w = w;

    aInputfield->bg = bg;
    aInputfield->fg = fg;

    aInputfield->pFont = pFont;

    UI_inputfieldRefreshTexture(pRend, aInputfield);
}