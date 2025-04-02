#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>

typedef struct Button* Button;

Button UI_buttonCreate();

void UI_buttonRenderer(SDL_Renderer* pRend, Button aButton);

void UI_buttonDestroy(Button aButton);

#endif