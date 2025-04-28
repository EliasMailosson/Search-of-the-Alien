#ifndef LABEL_H
#define LABEL_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>

typedef struct Label* Label;

Label UI_labelCreate();
void UI_labelDestroy(Label aLabel);

void UI_labelRender(SDL_Renderer* pRend, Label aLabel);

void UI_labelRefreshTexture(SDL_Renderer* pRend, Label aLabel);

// Setters
void UI_labelSetAppearance(SDL_Renderer* pRend, Label aLabel, int x, int y, SDL_Color color, TTF_Font* pFont);
void UI_labelSetText(Label aLabel, char* text);
void UI_labelGetText(Label aLabel, char* text);
void UI_labelSetPosition(Label aLabel, int x, int y);

// Getters
void UI_labelGetDimensions(Label aLabel, int* w, int* h);



#endif