#ifndef INPUTFIELD_H
#define INPUTFIELD_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>

typedef struct Inputfield* Inputfield;

Inputfield UI_inputfieldCreate();
void UI_inputfieldDestroy(Inputfield aInputfield);

void UI_inputfieldRender(SDL_Renderer* pRend, Inputfield aInputfield);

void UI_inputfieldRefreshTexture(SDL_Renderer* pRend, Inputfield aInputfield);

void UI_inputfieldSetFocus(Inputfield aInputfield, int mouseX, int mouseY);

bool UI_inputfieldIsFocused(Inputfield aInputfield);

void UI_inputfieldUpdateBuffer(Inputfield aInputfield, char *text);

void UI_inputfieldSetAppearance(SDL_Renderer* pRend, Inputfield aInputfield, int x, int y, int w, SDL_Color bg, SDL_Color fg, TTF_Font* pFont);

#endif