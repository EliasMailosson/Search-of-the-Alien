#ifndef MAP_H
#define MAP_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>

#define MAP_WIDTH 60
#define MAP_HEIGHT 60
#define LOBBY_HEIGHT 30
#define LOBBY_WIDTH 30
#define TILE_INDEX_COUNT (MAP_HEIGHT * MAP_WIDTH)+1
#define TILE_SIZE 128
#define TILE_SPRITE_SIZE 256
#define MAX_COUNT_SPRITE_TILES 30
#define FILE_PHAT_LOBBY_DATA "data/lobby.csv"
#define FILE_PHAT_LOBBY_SPRITE "assets/images/tiles/tileset-9-transparent.png"

#define LOGICAL_WIN_W 600
#define LOGICAL_WIN_H (600 * 16 / 9)


typedef struct Map *Map;

void MAP_MapRender(SDL_Renderer *pRend, Map aMap);

void MAP_TileRender(SDL_Renderer *pRend, Map aMap, int y, int x, SDL_Rect *currentRect);

Map MAP_MapCreate(SDL_Renderer *pRend, int winW, int winH);

void MAP_MapRefresh(Map aMap, int winW, int winH, SDL_Point cameraCenter);

void MAP_MapDestroy(Map aMap);

void MAP_MapGetTilseFromLobby(int tileID[MAP_HEIGHT][MAP_WIDTH]);

void MAP_TilesFillWithBlank(int tileID[MAP_HEIGHT][MAP_WIDTH]);


#endif