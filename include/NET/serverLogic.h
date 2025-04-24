#include "../../include/NET/server.h"
#include "../../include/MAP/map.h"

#include <math.h>


typedef struct ServerMap *ServerMap;

ServerMap NET_serverMapCreate();
bool isTileWalkable(ServerMap aMap, int x, int y);
void NET_serverMapDestroy(ServerMap aMap);
