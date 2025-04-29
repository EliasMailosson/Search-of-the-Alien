#include "server.h"
#include "../../include/MAP/map.h"

#include <math.h>

typedef struct server *Server;
typedef struct ServerMap *ServerMap;

ServerMap NET_serverMapCreate();
void MAP_ScreenToTile(ServerMap aServerMap, int screenX, int screenY, int *outTileX, int *outTileY);
bool MAP_TileNotWalkable(ServerMap aServerMap, int screenX, int screenY);
void NET_serverCheckPlayerCollision(Server aServer, int selfIdx, int *collide);
void NET_serverMapDestroy(ServerMap aMap);
void NET_serverMapGenerateNewMap(ServerMap aServerMap);

void NET_serverMapSetSeed(ServerMap aServerMap,uint32_t newSeed);
uint32_t NET_serverMapGetSeed(ServerMap aServerMap);
