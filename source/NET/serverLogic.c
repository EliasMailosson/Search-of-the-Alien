#include "../../include/NET/serverLogic.h"

struct ServerMap
{
    int MapTileId[MAP_HEIGHT][MAP_WIDTH];
};

ServerMap NET_serverMapCreate() {
    ServerMap aServerMap = malloc(sizeof *aServerMap);

    MAP_TilesFillWithBlank(aServerMap->MapTileId);
    MAP_MapGetTilesFromLobby(aServerMap->MapTileId);

    return aServerMap;
}

bool isTileWalkable(ServerMap aMap, int x, int y) {
    if (aMap->MapTileId[y][x] == -1) {
        return true;
    }
    else {
        return false;
    }
    
}

void NET_serverMapDestroy(ServerMap aMap) {
    free(aMap);
}