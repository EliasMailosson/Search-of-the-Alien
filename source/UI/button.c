#include "../../include/UI/button.h"

#include <stdlib.h>
#include <string.h>
typedef struct Button
{
    SDL_Rect buttonRect;
    SDL_Color backgroundColor;
} *Button;


Button UI_buttonCreate() {
    Button aButton = malloc(sizeof(struct Button));

    aButton->backgroundColor = (SDL_Color){ .r = 255, .g = 0, .b = 0, .a = 255 };
    aButton->buttonRect = (SDL_Rect){ .x = 0, .y = 0, .w = 50, .h = 50 };

    return aButton;
}

void UI_buttonRenderer(SDL_Renderer* pRend, Button aButton) {
    SDL_SetRenderDrawColor(pRend, aButton->backgroundColor.r, aButton->backgroundColor.g, aButton->backgroundColor.b, aButton->backgroundColor.a);
    SDL_RenderFillRect(pRend, &aButton->buttonRect);
}

void UI_buttonDestroy(Button aButton) {
    free(aButton);
}

