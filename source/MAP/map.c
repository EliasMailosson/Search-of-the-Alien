#include "../../include/MAP/map.h"

#include <stdbool.h>

#define MAP_WIDTH 6
#define MAP_HEIGHT 6
#define TILE_WIDTH 64
#define TILE_HEIGHT 64

typedef struct tile {               
    int tileID;
    bool walkable;
} Tile;

struct Map {
    SDL_Texture *texture;
    int tile_width;
    int tile_height;
    SDL_Rect tileIndex[2];
    Tile tiles[MAP_HEIGHT][MAP_WIDTH];
};

Map createMap(SDL_Renderer *pRend){
    Map aMap = malloc(sizeof(struct Map));
    if(aMap == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }

    // behöver vi gå upp i mappen?
    MAP_loadTileSheet(pRend, "assets/images/tiles/tileset-4.png", aMap);

    aMap->tile_width = TILE_WIDTH;
    aMap->tile_height = TILE_HEIGHT;

    aMap->tileIndex[0] = (SDL_Rect){.x = (64*7), .y = 0, .w = TILE_WIDTH, .h = TILE_HEIGHT};
    aMap->tileIndex[1] = (SDL_Rect){.x = (64*8), .y = 0, .w = TILE_WIDTH, .h = TILE_HEIGHT};

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            aMap->tiles[y][x].tileID = (x + y) % 2;   // alternate between tile 0 and 1
            aMap->tiles[y][x].walkable = true;        // or false for some
        }
    }

    return aMap;
}

void MAP_loadTileSheet(SDL_Renderer* pRend, char *imagePath, Map aMap) {
    SDL_Surface *surface = IMG_Load(imagePath);
    aMap->texture = SDL_CreateTextureFromSurface(pRend, surface);
    SDL_FreeSurface(surface);
    if(!aMap->texture){
        printf("Error creating texture for map %s\n", IMG_GetError());
    }
}

void destroyMap(Map aMap){
    SDL_DestroyTexture(aMap->texture);
    aMap->texture = NULL;
    free(aMap);
    aMap = NULL;
}