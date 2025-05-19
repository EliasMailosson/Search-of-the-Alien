#ifndef ENEMIES_H
#define ENEMIES_H

#include "../../include/NET/shared.h"
#include "../../include/NET/serverLogic.h"

#define MAX_ENEMIES_CLIENT_SIDE 25
#define MAX_ENEMIES_RANGE 1200

typedef struct enemy *Enemy;
typedef struct enemies* Enemies;
typedef struct ServerMap *ServerMap;
typedef struct server *Server;

typedef enum {
    LIGHT_ENEMY, 
    HEAVY_ENEMY, 
    BOSS_ENEMY
}EnemyID;

typedef struct {
    Enemy enemy;
    float dist;
} SortEntry;

//Enemies enemyCreate();
Enemies NET_enemiesCreate(void);
int NET_enemiesPush(Enemies aE, Enemy new);
Enemy NET_enemiesPopAt(Enemies aE, size_t index);
// Enemy NET_enemiesPopAt(Enemies aE, size_t index);
Enemy NET_enemyCreate(int pixelX, int pixelY, EnemyID id, const int difficulty);
Enemy NET_enemiesGetAt(Enemies aE, size_t index);
void NET_enemiesDestroy(Enemies aEnemies);
size_t NET_enemiesGetSize(Enemies aEnemies);
void NET_enemiesClear(Enemies aEnemies);

int NET_enemiesCompEntries(const void *a, const void *b);
float NET_enemiesCompute_dist(const Enemy e, SDL_Rect playerRect);
SDL_Point NET_enemyGetPos(Enemy aEnemy);
int NET_enemyGetDirection(Enemy aEnemy);
SDL_Rect NET_getEnemySpawnZone(SDL_Rect playerRect, int tile);

SDL_Point NET_enemyGetPoint(Enemies aEnemies, int index);

void NET_checkEnemyCollision(Enemies aEnemies, int index, int *collide);
SDL_Rect NET_enemyGetHitbox(Enemies aEnemies, int index);
// int GetEnemyCount(Enemies aEnemies);
void NET_setEnemyHitbox(Enemies aEnemies, int enemyindex, SDL_Rect H);

void NET_playerTracker(Enemies aEnemies, Server aServer, int playerIndex, int enemyIndex);

void NET_enemyAngleTracker(Enemies aEnemies, SDL_Point playerPos, int index);

float NET_enemyGetAngle(Enemies aEnemies, int index);

Uint32 NET_enemyGetAttackTime(Enemies aEnemies, int index);

void NET_enemySetAttackTime(Enemies aEnemies, int index);

// SDL_Rect enemyGetRect(Enemies aEnemies, int index);

int NET_enemyDamaged(Enemies aEnemies, int damage, int index, int *pEnemyCount);

int NET_enemyGetCount(Enemies aEnemies);

bool NET_enemyColitino(SDL_Rect A,SDL_Rect B);

int NET_getEnemyHP(Enemy aEnemy);


#endif