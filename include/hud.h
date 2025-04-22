#ifndef HUD_H
#define HUD_H
#include "../include/NET/client.h"
#include "SDL_image.h"

#define SDL_WHITE (SDL_Color){255, 255, 255, SDL_ALPHA_OPAQUE}
#define SDL_RED (SDL_Color){255, 0, 0, SDL_ALPHA_OPAQUE}
#define SDL_GREEN (SDL_Color){0, 255, 0, SDL_ALPHA_OPAQUE}
#define SDL_CYAN (SDL_Color){0, 255, 255, SDL_ALPHA_OPAQUE}
#define SDL_YELLOW (SDL_Color){255, 255, 0, SDL_ALPHA_OPAQUE}
#define SDL_BLUE (SDL_Color){0, 0, 255, SDL_ALPHA_OPAQUE}
#define SDL_BLACK (SDL_Color){0, 0, 0, SDL_ALPHA_OPAQUE}
#define SDL_TRANSPARENT (SDL_Color){0, 0, 0, SDL_ALPHA_TRANSPARENT}

#define FILE_PHAT_ARROW "assets/images/player/testArrow.png"

typedef struct Hud *Hud;
typedef struct Arrow *Arrow;

Hud hudCreate(SDL_Renderer *pRend,SDL_Window *pWin);
Hud hudDestroy(Hud aHud);

Arrow arrowCreate(int index, SDL_Renderer *pRend,SDL_Window *pWindow);


#endif