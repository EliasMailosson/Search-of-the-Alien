#ifndef MAP_H
#define MAP_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>

#define MAP_WIDTH 30
#define MAP_HEIGHT 30
#define TILE_INDEX_COUNT (MAP_HEIGHT * MAP_WIDTH)+1
#define TILE_SIZE 64
#define TILE_SPRITE_SIZE 256
#define MAX_COUNT_TILES 25
#define FILE_PHAT_LOBBY_DATA "data/lobby.csv"
#define FILE_PHAT_LOBBY_SPRITE "assets/images/tiles/new-tileset-9.png"

typedef struct tile Tile;

typedef struct Map *Map;

void MAP_MapRender(SDL_Renderer *pRend, Map aMap);

void MAP_TileRender(SDL_Renderer *pRend, Map aMap, int y, int x);

Map MAP_MapCreate(SDL_Renderer *pRend, int posX, int posY);

Tile **MAP_TileCreate(int posX, int posY);

void MAP_MapRefresh(Map aMap, int posX, int posY);

void MAP_TileSheetload(SDL_Renderer* pRend, char *imagePath, Map aMap);

void MAP_MapDestroy(Map aMap);

void MAP_MapGetTilseFromLobby(Tile **ppTiles);


#endif