#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include "SDL_image.h"

typedef struct tile Tiles;

typedef struct Map *Map;

void MAP_MapRender(SDL_Renderer *pRend, Map aMap);

void MAP_TileRender(SDL_Renderer *pRend, Map aMap, int y, int x);

Map MAP_MapCreate(SDL_Renderer *pRend, int posX, int posY);

void MAP_MapRefresh(Map aMap, int posX, int posY);

void MAP_TileSheetload(SDL_Renderer* pRend, char *imagePath, Map aMap);

void MAP_MapDestroy(Map aMap);

#endif