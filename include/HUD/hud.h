#ifndef HUD_H
#define HUD_H
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "../../include/NET/client.h"
#include "../../include/MAP/map.h"

#define FILE_PHAT_ARROW_RED "assets/images/player/arrow_red.png"
#define FILE_PHAT_ARROW_BLUE "assets/images/player/arrow_blue.png"
#define FILE_PHAT_ARROW_GREEN "assets/images/player/arrow_green.png"
#define FILE_PHAT_ARROW_YELLO "assets/images/player/arrow_yellow.png"
#define FILE_PHAT_ARROW_CYAN "assets/images/player/arrow_cyan.png"
#define FILE_PHAT_ARROW_MAGENTA "assets/images/player/arrow_magenta.png"
#define FILE_PHAT_ARROW_ORANGE "assets/images/player/arrow_orange.png"
#define FILE_PHAT_ARROW_PURPLE "assets/images/player/arrow_purple.png"
#define FILE_PHAT_ARROW_BLACK "assets/images/player/arrow.png"

#define ARROW_SIZE (TILE_SIZE / 2.0f)
#define OBJECTIVECOUNT 3

typedef struct Hud *Hud;
typedef struct Arrow *Arrow;

Hud hudCreate(SDL_Renderer *pRend);
void hudDestroy(Hud aHud);
void hudRender(Client aClient, Hud aHud,SDL_Renderer *pRend, int windowW, int windowH);

Arrow arrowCreate();
void updateArrows(Hud aHud,SDL_Window *pWin,Client aClient, SDL_Point playerPos[MAX_CLIENTS]);
SDL_Point hudGettArrowPos(Hud aHud, int index);
void updateHudPlayerList(Client aClient, Hud aHud, SDL_Renderer *pRend, int windowW, int windowH);

bool pointInRect(SDL_Rect rect, SDL_Point point);

#endif