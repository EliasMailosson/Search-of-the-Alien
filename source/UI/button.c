#include "../../include/UI/button.h"
#include "../../include/UI/button.h"
#include <SDL.h>
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

void UI_buttonSetAppearence(Button aButton, SDL_Color backgroundColor) {
    aButton->backgroundColor = backgroundColor;
    aButton->buttonRect = (SDL_Rect){ .x = , .y = aButton. }
}
