#ifndef MAP_H
#define MAP_H

#include "../../include/game.h"

typedef struct tile Tiles;

typedef struct Map *Map;

Map createMap(SDL_Renderer *pRend);

void MAP_loadTileSheet(SDL_Renderer* pRend, char *imagePath, Map aMap);

void destroyMap(Map aMap);

#endif