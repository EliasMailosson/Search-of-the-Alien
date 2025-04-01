#ifndef PANEL_H
#define PANEL_H

#include <stdbool.h>
#include <SDL.h>

typedef struct Panel *Panel;

Panel UI_panelCreate();

void UI_panelSetAppearance(Panel aPanel, SDL_Rect rect, SDL_Color src_bg);

void UI_panelRender(SDL_Renderer *pRend, Panel aPanel);

#endif