#include "../../include/NET/server.h"
#include "../../include/MAP/map.h"

#include <math.h>


typedef struct ServerMap *ServerMap;

ServerMap NET_serverMapCreate();
void MAP_ScreenToTile(ServerMap aServerMap, int screenX, int screenY, int *outTileX, int *outTileY);
bool MAP_TileNotWalkable(ServerMap aServerMap, int screenX, int screenY);
void NET_serverMapDestroy(ServerMap aMap);
