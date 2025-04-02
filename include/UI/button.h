#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>
#include <SDL_ttf.h>

typedef struct Button* Button;

Button UI_buttonCreate();

void UI_buttonDimensions(Button aButton, int newX, int newY, int newWidth, int newHeight);

void UI_buttonSetText(Button aButton, char* text);

void UI_buttonSetTextPosition(Button aButton);

void UI_setButtonLabelappearence(SDL_Renderer* pRend, Button aButton, SDL_Color color, TTF_Font* pFont);

void UI_buttonRenderer(SDL_Renderer* pRend, Button aButton);

void UI_buttonDestroy(Button aButton);

#endif