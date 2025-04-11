#include "../../include/MAP/map.h"

#include <stdbool.h>

#define MAP_WIDTH 30
#define MAP_HEIGHT 30
#define TILE_SIZE 64

typedef struct tile { 
    SDL_Rect tileRect;              
    int tileID;
    bool walkable;
} Tile;

struct Map {
    SDL_Texture *texture;
    int tile_width;
    int tile_height;
    SDL_Rect tileIndex[3];
    Tile tiles[MAP_HEIGHT][MAP_WIDTH];
};

void MAP_RenderMap(SDL_Renderer *pRend, Map aMap) {
    
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            MAP_RenderTiles(pRend, aMap, y, x);
        }
    }

    //red lines
    SDL_SetRenderDrawColor(pRend, 255, 0, 0, 255);  // red
    //SDL_RenderDrawLine(pRend, 0, pRend.windowHeight / 2, pView->windowWidth / 2, pView->windowHeight / 2);   // diagonal line
    //SDL_RenderDrawLine(pRend, pView->windowWidth / 2, 0, pView->windowWidth / 2, pView->windowHeight / 2);   // vertical line
}

void MAP_RenderTiles(SDL_Renderer *pRend, Map aMap, int y, int x) {
    SDL_RenderCopy(pRend, aMap->texture, &aMap->tileIndex[aMap->tiles[y][x].tileID], &aMap->tiles[y][x].tileRect);
}

Map MAP_CreateMap(SDL_Renderer *pRend, int posX, int posY){
    Map aMap = malloc(sizeof(struct Map));
    if(aMap == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }

    // behöver vi gå upp i mappen?
    MAP_loadTileSheet(pRend, "assets/images/tiles/tileset-4.png", aMap);

    aMap->tile_width = TILE_SIZE;
    aMap->tile_height = TILE_SIZE;

    aMap->tileIndex[0] = (SDL_Rect){.x = (TILE_SIZE*7), .y = 0, .w = TILE_SIZE, .h = TILE_SIZE};
    aMap->tileIndex[1] = (SDL_Rect){.x = (TILE_SIZE*8), .y = 0, .w = TILE_SIZE, .h = TILE_SIZE};
    aMap->tileIndex[2] = (SDL_Rect){.x = (TILE_SIZE*0), .y = 0, .w = TILE_SIZE, .h = TILE_SIZE};


    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            aMap->tiles[y][x].tileID = (x + y) % 2;   // alternate between tile 0 and 1
            aMap->tiles[y][x].walkable = true;        // or false for some
            aMap->tiles[y][x].tileRect.w = TILE_SIZE;
            aMap->tiles[y][x].tileRect.h = TILE_SIZE;
            aMap->tiles[y][x].tileRect.x = (int)((x - y) * (TILE_SIZE/2) + (posX/2) - TILE_SIZE/2); 
            aMap->tiles[y][x].tileRect.y = (int)((x + y) * (TILE_SIZE/4) + (posY / 2) - (TILE_SIZE / 4) - (TILE_SIZE * MAP_HEIGHT / 4));
        }
    }
    return aMap;
}

void MAP_refreshMap(Map aMap, int posX, int posY){

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            aMap->tiles[y][x].tileRect.x = (int)((x - y) * (TILE_SIZE/2) + (posX/2) - TILE_SIZE/2); 
            aMap->tiles[y][x].tileRect.y = (int)((x + y) * (TILE_SIZE/4) + (posY/2) - (TILE_SIZE / 4) - (TILE_SIZE * MAP_HEIGHT / 4));
        }
    }
}

void MAP_loadTileSheet(SDL_Renderer* pRend, char *imagePath, Map aMap) {
    SDL_Surface *surface = IMG_Load(imagePath);
    aMap->texture = SDL_CreateTextureFromSurface(pRend, surface);
    SDL_FreeSurface(surface);
    if(!aMap->texture){
        printf("Error creating texture for map %s\n", IMG_GetError());
    }
}

void MAP_DestroyMap(Map aMap){
    SDL_DestroyTexture(aMap->texture);
    aMap->texture = NULL;
    free(aMap);
    aMap = NULL;
}