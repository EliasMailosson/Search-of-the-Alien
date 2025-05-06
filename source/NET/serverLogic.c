#include "../../include/NET/serverLogic.h"
#include "../../include/NET/server.h"

#define PROJ_DESPAWN_DISTANCE 10000

struct ServerMap
{
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

void NET_serverMapSetEdgesToZero(ServerMap aServerMap){
    for (int i = 0; i < MAP_WIDTH; i++){
        aServerMap->MapTileId[0][i] = 0;
        aServerMap->MapTileId[MAP_HEIGHT-1][i] = 0;
    }
    for (int i = 0; i < MAP_HEIGHT; i++){
        aServerMap->MapTileId[i][0] = 0;
        aServerMap->MapTileId[i][MAP_WIDTH - 1] = 0;
    }
}


void NET_serverMapSetSeed(ServerMap aServerMap,uint32_t newSeed){
    aServerMap->seed = newSeed;
}

uint32_t NET_serverMapGetSeed(ServerMap aServerMap){
    return aServerMap->seed;
}

void NET_serverMapGenerateNewMap(ServerMap aServerMap){
    MAP_generatePerlinNoise(
        aServerMap->MapTileId, MAP_HEIGHT, MAP_WIDTH, 
        RANGE, MAP_OFFSET, aServerMap->seed);
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

bool NET_serverFindSpawnTile(ServerMap aServerMap, int *freekoordX, int *freekoordY) {
    const int maxAttempts = 1000;

    for (int attempt = 0; attempt < maxAttempts; attempt++) {
        int x = rand() % MAP_WIDTH;
        int y = rand();

        if (aServerMap->MapTileId[y][x] != 0) {
            *freekoordX = x;
            *freekoordY= y;
            return true;
        }
    }
    return false;
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

void NET_projectileSpawn(Server aServer, Projectile *list, uint8_t srcPlayerIdx) {
    int projCount = NET_serverGetProjCount(aServer);
    SDL_Rect hb = NET_serverGetPlayerHitbox(aServer, srcPlayerIdx);
    list[projCount].x = hb.x;
    list[projCount].y = hb.y;
    list[projCount].angle = NET_serverGetPlayerAngle(aServer, srcPlayerIdx);
    list[projCount].srcPlayerIdx = srcPlayerIdx;

    if(projCount < MAX_SERVER_PROJECTILES) NET_serverSetProjCount(aServer, ++projCount);
}

void NET_projectileKill(Server aServer, Projectile *list, int projIdx) {
    int projCount = NET_serverGetProjCount(aServer);
    for(int i = projIdx; i < projCount-1; i++) {
        list[i] = list[i+1];
    }
    NET_serverSetProjCount(aServer, --projCount);
}

void NET_projectilesUpdate(Server aServer, Projectile *list) {
    int projCount = NET_serverGetProjCount(aServer);
    for(int i = 0; i < projCount; i++) {
        float speed = (float)NET_serverGetProjectileSpeed(aServer, list[i].srcPlayerIdx);

        float r = ((float)list[i].angle / 255.0f) * (2.0f * M_PI);
        float dx = cosf(r);
        float dy = sinf(r);

        float moveX = dx * speed * -1;
        float moveY = dy * speed * -1;

        list[i].x += moveX;
        list[i].y += moveY;

        if(abs(list[i].x) > PROJ_DESPAWN_DISTANCE || abs(list[i].y) > PROJ_DESPAWN_DISTANCE) {
            NET_projectileKill(aServer, list, i);
        }
    }
}

int getTileHeight(ServerMap aServerMap){
    return aServerMap->tileRect.h;
}