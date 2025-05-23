#ifndef SERVER_H
#define SERVER_H
#include "../../include/NET/shared.h"
#include "../../include/NET/protocol.h"
#include "../../include/NET/enemies.h"
#include "../../include/NET/serverLogic.h"
//#include "../../include/CONCURRENCY/semaphore.h"
#include "../../include/CONCURRENCY/threads.h"
#include <math.h>
#include <limits.h>

#define MAX_COLORS 8
#define MAX_SERVER_PROJECTILES 512
#define CLIENT_PROJ_RANGE 1200

typedef struct Player Player;
typedef struct Weapon Weapon;
typedef struct Scenario Scenario;
typedef struct enemies *Enemies;
typedef struct User User;
struct server;
typedef struct server *Server;

/** Allocating memory for server
 * UDP connection
 */
Server NET_serverCreate();

//server send
void NET_serverSendInt(Server aServer,GameState GS, MessageType msgType,int placeHolder, int index);
void NET_serverSendString(Server aServer,GameState GS, MessageType msgType, const char* str, int index);
void NET_serverSendArray(Server aServer,GameState GS, MessageType msgType, const void* array, Uint32 arraySize, int index);

void NET_serverSendPlayerPacket(Server aServer,GameState GS);
void NET_serverSendProjPacket(Server aServer);

//server respons
void NET_serverClientConnected(Packet aPacket, Server aServer);
void NET_serverClientDisconnect(Server aServer);
void NET_serverChangeGameStateOnClient(Server aServer,Packet aPacket);

void NET_serverUpdatePlayer(Server aServer, Packet aPacket,GameState state);

SDL_Rect NET_serverGetPlayerHitbox(Server aServer, int playerIndex);
int NET_serverGetClientCount(Server aServer);
void NET_serverSetPlayerHitbox(Server aServer, int playerIndex, SDL_Rect r);
void NET_serverSetNewMap(Server aServer);
int NET_serverGetProjectileSpeed(Server aServer, int playerIdx);

//server update
bool enemyAttackPlayer(Server aServer, int index, SDL_Rect enemyHitbox);
void NET_serverUpdateEnemies(Server aServer, Enemies aEnemies, ServerMap aMap);
void NET_serverSendEnemiesPacket(Server aServer, GameState GS, Enemies aEnemies);
void NET_serverScenarioCheckForVictory(Server aServer);
void NET_serverEnemiesSpawnInterval(Server aServer);


//server util
int NET_serverFindPlayerID(Server aServer, const char* str);
int NET_serverCompIP(Server aServer);
void NET_serverRemoveUser(Server aServer,int index);
void NET_serverAddUser(Server aServer, User newUser);
int NET_serverGetProjCount(Server aServer);
float NET_serverGetPlayerAngle(Server aServer, int playerIdx);
void NET_serverSetProjCount(Server aServer, int count);
void NET_serverScenarioUpdate(Scenario *s, ScenarioState state, uint32_t seed);
uint8_t NET_serverGetPercentage(int currentHP, int maxHP);
void NET_serverForceGameStateChange(Server aServer, GameState state, int index);

/** Freeing memory after use, for server */
void NET_serverDestroy(Server aServer);

int NET_serverAssignColorIndex(Server aServer);

#endif