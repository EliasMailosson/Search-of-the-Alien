#include "../../include/NET/serverLogic.h"

struct ServerMap
{
    int MapTileId[MAP_HEIGHT][MAP_WIDTH];
    SDL_Rect tileRect;
};

ServerMap NET_serverMapCreate() {
    ServerMap aServerMap = malloc(sizeof *aServerMap);

    MAP_TilesFillWithBlank(aServerMap->MapTileId);
    MAP_MapGetTilesFromLobby(aServerMap->MapTileId);

    aServerMap->tileRect = (SDL_Rect) {.x = 0, .y = 0, .w = TILE_SIZE, .h = TILE_SIZE};

    return aServerMap;
}


void MAP_ScreenToTile(ServerMap aServerMap, int screenX, int screenY, int *outTileX, int *outTileY) {
    float dx = screenX - aServerMap->tileRect.x;
    float dy = screenY - aServerMap->tileRect.y;

    float a = dx / (aServerMap->tileRect.w * 0.5f);
    float b = dy / (aServerMap->tileRect.h * 0.25f);

    float fx = (a + b) * 0.5f;
    float fy = (b - a) * 0.5f; 

    int tx = (int)floor(fx);
    int ty = (int)floor(fy);

    *outTileX = tx;
    *outTileY = ty;
}

bool MAP_TileNotWalkable(ServerMap aServerMap, int screenX, int screenY) {
    int tileX, tileY;
    MAP_ScreenToTile(aServerMap, screenX, screenY, &tileX, &tileY);
    printf("tileY: %d, tileX: %d ,tileID:%d\n", tileY, tileX, aServerMap->MapTileId[tileX][tileY]);
    if (aServerMap->MapTileId[tileY][tileX] == 0){
        return true;
    }
    else 
    {
        return false;
    }
}


void NET_serverMapDestroy(ServerMap aMap) {
    free(aMap);
}