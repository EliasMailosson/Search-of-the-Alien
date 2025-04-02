#include "../../include/UI/button.h"
#include "../../include/UI/label.h"

#include <stdlib.h>
#include <string.h>
typedef struct Button
{
    SDL_Rect buttonRect;
    SDL_Color backgroundColor;
    Label buttonText;
} *Button;

Button UI_buttonCreate() {
    Button aButton = malloc(sizeof(struct Button));

    aButton->backgroundColor = (SDL_Color){ .r = 255, .g = 0, .b = 0, .a = 255 };
    aButton->buttonRect = (SDL_Rect){ .x = 300, .y = 10, .w = 150, .h = 100 };
    aButton->buttonText = UI_labelCreate();
    UI_buttonSetText(aButton, "hej");

    return aButton;
}

void UI_buttonDimensions(Button aButton, int newX, int newY, int newWidth, int newHeight) {
    aButton->buttonRect.x = newX;
    aButton->buttonRect.y = newY;
    aButton->buttonRect.w = newWidth;
    aButton->buttonRect.h = newHeight;
}

void UI_buttonSetText(Button aButton, char* text) {
    UI_labelSetText(aButton->buttonText, text);
}

void UI_buttonSetTextPosition(Button aButton) {
    int textWidth = 0, textHeight = 0, xPos = 0, yPos = 0;
    UI_labelGetDimensions(aButton->buttonText, &textWidth, &textHeight);
    xPos = (aButton->buttonRect.x) + (aButton->buttonRect.w / 2) - (textWidth / 2);
    yPos = (aButton->buttonRect.y) + (aButton->buttonRect.h / 2) - (textHeight / 2);
    UI_labelSetPosition(aButton->buttonText, xPos, yPos);
}

void UI_setButtonLabelappearence(SDL_Renderer* pRend, Button aButton, SDL_Color color, TTF_Font* pFont) {
    UI_labelSetAppearance(pRend, aButton->buttonText, aButton->buttonRect.x, aButton->buttonRect.y, color, pFont);
    UI_buttonSetTextPosition(aButton);
    UI_labelRefreshTexture(pRend, aButton->buttonText);
}

void UI_buttonRenderer(SDL_Renderer* pRend, Button aButton) {
    SDL_SetRenderDrawColor(pRend, aButton->backgroundColor.r, aButton->backgroundColor.g, aButton->backgroundColor.b, aButton->backgroundColor.a);
    SDL_RenderFillRect(pRend, &aButton->buttonRect);

    UI_labelRender(pRend, aButton->buttonText);
}

void UI_buttonDestroy(Button aButton) {
    UI_labelDestroy(aButton->buttonText);
    free(aButton);
}

