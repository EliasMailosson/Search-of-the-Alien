#include "../../include/UI/button.h"
#include "../../include/UI/label.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
typedef struct Button
{
    SDL_Rect buttonRect;
    SDL_Color backgroundColor;
    Label buttonText;
    bool isHovered;
} *Button;

Button UI_buttonCreate() {
    Button aButton = malloc(sizeof(struct Button));

    aButton->backgroundColor = (SDL_Color){ .r = 255, .g = 0, .b = 0, .a = 255 };
    aButton->buttonRect = (SDL_Rect){ .x = 300, .y = 10, .w = 150, .h = 100 };
    aButton->buttonText = UI_labelCreate();
    UI_buttonSetText(aButton, "hej");
    aButton->isHovered = false;

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

void UI_buttonSetLabelappearence(SDL_Renderer* pRend, Button aButton, SDL_Color textColor, TTF_Font* pFont, SDL_Color buttonColor) {
    UI_labelSetAppearance(pRend, aButton->buttonText, aButton->buttonRect.x, aButton->buttonRect.y, textColor, pFont);
    UI_buttonSetTextPosition(aButton);
    aButton->backgroundColor = buttonColor;
    UI_labelRefreshTexture(pRend, aButton->buttonText);
}

void UI_buttonRenderer(SDL_Renderer* pRend, Button aButton) {
    if(aButton->isHovered) {
        SDL_SetRenderDrawColor(pRend, aButton->backgroundColor.r-70, aButton->backgroundColor.g-70, aButton->backgroundColor.b-70, aButton->backgroundColor.a-50);
    }
    else SDL_SetRenderDrawColor(pRend, aButton->backgroundColor.r, aButton->backgroundColor.g, aButton->backgroundColor.b, aButton->backgroundColor.a);
    SDL_RenderFillRect(pRend, &aButton->buttonRect);

    UI_labelRender(pRend, aButton->buttonText);
}

bool UI_buttonIsHovered(Button aButton, int mouseX, int mouseY) {
    if (mouseX >= aButton->buttonRect.x && mouseX <= (aButton->buttonRect.x + aButton->buttonRect.w) 
    && mouseY >= aButton->buttonRect.y && mouseY <= (aButton->buttonRect.y + aButton->buttonRect.h)) {
        aButton->isHovered = true;
        return true;
    }
    else {
        aButton->isHovered = false;
        return false;
    }
}

void UI_buttonConfigure(Button aButton, char* text, int newX, int newY, int newWidth, int newHeight, SDL_Renderer* pRend, SDL_Color textColor, TTF_Font* pFont, SDL_Color buttonColor) {
    UI_buttonSetText(aButton, text);
    UI_buttonDimensions(aButton, newX, newY, newWidth, newHeight);
    UI_buttonSetLabelappearence(pRend, aButton, textColor, pFont, buttonColor);
}

void UI_buttonDestroy(Button aButton) {
    UI_labelDestroy(aButton->buttonText);
    free(aButton);
}

