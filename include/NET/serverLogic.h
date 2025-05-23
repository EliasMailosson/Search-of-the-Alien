#ifndef SERVERLOGIC_H
#define SERVERLOGIC_H

#include "server.h"
#include "../../include/MAP/map.h"

#include <math.h>

#define DEFAULT_PROJECTILE_SPEED 15
typedef struct __attribute__((packed)) Projectile {
    int16_t x, y;
    int8_t angle;
    uint8_t srcPlayerIdx;
}Projectile;

typedef struct server *Server;
typedef struct ServerMap *ServerMap;

ServerMap NET_serverMapCreate();
void MAP_ScreenToTile(ServerMap aServerMap, int screenX, int screenY, int *outTileX, int *outTileY);
bool MAP_TileNotWalkable(ServerMap aServerMap, int screenX, int screenY, GameState state);
bool NET_findEnemySpawnPoint(SDL_Rect spawnZone, SDL_Rect *otherZones, int otherZoneCount, int *outX, int *outY);
bool NET_serverSpawnForObj(ServerMap aServerMap, int *freekoordX, int *freekoordY);
void NET_serverCheckPlayerCollision(Server aServer, int selfIdx, int *collide);
void NET_projectileSpawn(Server aServer, Projectile *list, uint8_t srcPlayerIdx);
void NET_projectileKill(Server aServer, Projectile *list, int projIdx);
void NET_projectilesUpdate(Server aServer, Projectile *list);
void NET_serverMapDestroy(ServerMap aMap);
void NET_serverMapGenerateNewMap(ServerMap aServerMap);

void NET_serverMapSetSeed(ServerMap aServerMap,uint32_t newSeed);
uint32_t NET_serverMapGetSeed(ServerMap aServerMap);
void NET_serverMapSetEdgesToZero(ServerMap aServerMap);

int getTileHeight(ServerMap aServerMap);

#endif