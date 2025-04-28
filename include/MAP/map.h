#ifndef MAP_H
#define MAP_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <math.h>

#define MAP_WIDTH 60
#define MAP_HEIGHT 60
#define MAP_TOTAL_SIZE (MAP_WIDTH * MAP_HEIGHT)

#define LOBBY_HEIGHT 30
#define LOBBY_WIDTH 30
#define TILE_INDEX_COUNT (MAP_HEIGHT * MAP_WIDTH)+1
#define TILE_SIZE 128
#define TILE_SPRITE_SIZE 256
#define MAX_COUNT_SPRITE_TILES 40
#define FILE_PHAT_LOBBY_DATA "data/lobby.csv"
#define FILE_PHAT_LOBBY_SPRITE "assets/images/tiles/tileset-9-transparent.png"

#define LOGICAL_WIN_W 600
#define LOGICAL_WIN_H (600 * 16 / 9)

#define MAX_LUT_LEN 16

typedef enum {LOBBY_LUT,NEMUR_LUT,AURANTIC_LUT,CINDORA_LUT}PlanetLUT;

typedef struct Map *Map;

void MAP_convertTiles(int tileID[MAP_HEIGHT][MAP_WIDTH],PlanetLUT plantet);
void MAP_mapSetEdgesToZero(int tileID[][MAP_WIDTH]);
PlanetLUT MAP_mapGetPlanetLUT(Map aMap);

void MAP_MapRender(SDL_Renderer *pRend, Map aMap);

void MAP_TileRender(SDL_Renderer *pRend, Map aMap, int y, int x, SDL_Rect *currentRect);

Map MAP_MapCreate(SDL_Renderer *pRend, int winW, int winH);

SDL_Rect MAP_getTileRect(Map aMap);

void MAP_MapRefresh(Map aMap, int winW, int winH);

void MAP_MapDestroy(Map aMap);

//void MAP_MapGettOffset(int winH, int winW, SDL_Point *index);
void MAP_TilesFillWithBlank(int tileID[MAP_HEIGHT][MAP_WIDTH]);
void MAP_MapGetTilesFromLobby(int tileID[MAP_HEIGHT][MAP_WIDTH]);

void MAP_MapMoveMap(Map aMap, SDL_Point playerOffset);


#endif