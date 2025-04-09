#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include "SDL2/SDL_image.h"

typedef struct tile Tiles;

typedef struct Map *Map;

void MAP_RenderMap(SDL_Renderer *pRend, Map aMap);

void MAP_RenderTiles(SDL_Renderer *pRend, Map aMap, int y, int x);

Map MAP_CreateMap(SDL_Renderer *pRend);

void MAP_loadTileSheet(SDL_Renderer* pRend, char *imagePath, Map aMap);

void MAP_DestroyMap(Map aMap);

#endif