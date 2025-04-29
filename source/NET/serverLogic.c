#include "../../include/NET/serverLogic.h"
struct ServerMap{
    int MapTileId[MAP_HEIGHT][MAP_WIDTH];
    SDL_Rect tileRect;
    uint32_t seed;
};

ServerMap NET_serverMapCreate() {
    ServerMap aServerMap = malloc(sizeof *aServerMap);

    MAP_TilesFillWithBlank(aServerMap->MapTileId);
    MAP_MapGetTilesFromLobby(aServerMap->MapTileId);

    aServerMap->tileRect = (SDL_Rect) {.x = 0, .y = 0, .w = TILE_SIZE, .h = TILE_SIZE};

    return aServerMap;
}

void NET_serverMapSetSeed(ServerMap aServerMap,uint32_t newSeed){
    aServerMap->seed = newSeed;
}

uint32_t NET_serverMapGetSeed(ServerMap aServerMap){
    return aServerMap->seed;
}

void NET_serverMapGenerateNewMap(ServerMap aServerMap){
    MAP_generatePerlinNoise(aServerMap->MapTileId,MAP_HEIGHT,MAP_WIDTH,13,0,aServerMap->seed);
    for (int y = 0; y < MAP_HEIGHT; y++){
        for(int x = 0; x < MAP_WIDTH; x++){
            printf("%d,",aServerMap->MapTileId[y][x]);
        }printf("\n");
    }
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
    // printf("tileY: %d, tileX: %d ,tileID:%d\n", tileY, tileX, aServerMap->MapTileId[tileX][tileY]);
    if (aServerMap->MapTileId[tileY][tileX] == 0){
        return true;
    }
    else 
    {
        return false;
    }
}

void NET_serverCheckPlayerCollision(Server aServer, int selfIdx, int *collide) {
    int playerCount = NET_serverGetClientCount(aServer);
    float resistance = 1.0f;

    *collide = 0;
    for (int i = 0; i < playerCount; i++) {
        for (int j = i + 1; j < playerCount; j++) {
            SDL_Rect p1 = NET_serverGetPlayerHitbox(aServer, i);
            SDL_Rect p2 = NET_serverGetPlayerHitbox(aServer, j);
            p1.x;
            p2.x;

            float p1CenterX = p1.x + p1.w / 2.0f;
            float p1CenterY = p1.y + p1.h / 2.0f;
            float p2CenterX = p2.x + p2.w / 2.0f;
            float p2CenterY = p2.y + p2.h / 2.0f;

            bool isIntersect = (
                p1.x < p2.x + p2.w &&
                p1.x + p1.w > p2.x &&
                p1.y < p2.y + p2.h &&
                p1.y + p1.h > p2.y
            );
            if(isIntersect) {
                float intersectX = (p1.w / 2.0f + p2.w / 2.0f) - fabs(p1CenterX - p2CenterX);
                float intersectY = (p1.h / 2.0f + p2.h / 2.0f) - fabs(p1CenterY - p2CenterY);

                if (intersectX < intersectY) {
                    if((selfIdx == i || selfIdx == j) && *collide == 0) *collide = 1;
                    // float moveX = intersectX / 8.0f;
                    if (p1CenterX < p2CenterX) {
                        p1.x -= (int)ceil(resistance);
                        p2.x += (int)ceil(resistance);
                    } else {
                        p1.x += (int)ceil(resistance);
                        p2.x -= (int)ceil(resistance);
                    }
                } else {
                    if((selfIdx == i || selfIdx == j) && *collide == 0) *collide = 1;
                    // float moveY = intersectY / 8.0f;
                    if (p1CenterY < p2CenterY) {
                        p1.y -= (int)ceil(resistance);
                        p2.y += (int)ceil(resistance);
                    } else {
                        p1.y += (int)ceil(resistance);
                        p2.y -= (int)ceil(resistance);
                    }
                }

                NET_serverSetPlayerHitbox(aServer, i, p1);
                NET_serverSetPlayerHitbox(aServer, j, p2);
            }
        }
    }
}

void NET_serverMapDestroy(ServerMap aMap) {
    free(aMap);
}