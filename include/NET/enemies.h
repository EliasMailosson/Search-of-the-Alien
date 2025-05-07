#ifndef ENEMIES_H
#define ENEMIES_H

#include "../../include/NET/shared.h"
#include "../../include/NET/serverLogic.h"

#define MAX_ENEMIES_CLIENT_SIDE 25

typedef struct enemy *Enemy;
typedef struct enemies* Enemies;
typedef struct ServerMap *ServerMap;
typedef struct server *Server;

typedef enum {
    LIGHT_ENEMY, 
    HEAVY_ENEMY, 
    BOSS_ENEMY
}EnemyID;

//Enemies enemyCreate();
Enemies NET_enemiesCreate(void);
int NET_enemiesPush(Enemies aE, Enemy new);
Enemy NET_enemiesPopAt(Enemies aE, size_t index);
Enemy NET_enemiesPopAt(Enemies aE, size_t index);
Enemy NET_enemyCreate(int pixelX, int pixelY, EnemyID id, const int difficulty);
Enemy NET_enemiesGetAt(Enemies aE, size_t index);
void NET_enemiesDestroy(Enemies aEnemies);
size_t NET_enemiesGetSize(Enemies aEnemies);


SDL_Point enemyGetPoint(Enemies aEnemies, int index);

void checkEnemyCollision(Enemies aEnemies, int enemyindex, int *collide);
SDL_Rect enemyGetHitbox(Enemies aEnemies, int index);
// int GetEnemyCount(Enemies aEnemies);
void SetEnemyHitbox(Enemies aEnemies, int enemyindex, SDL_Rect H);

void PlayerTracker(Enemies aEnemies, Server aServer,int playerindex, int enemyindex);

void enemyAngleTracker(Enemies aEnemies, SDL_Point playerPos, int enemyIndex);

int enemyGetDirection(Enemies aEnemies, int index);

float enemyGetAngle(Enemies aEnemies, int index);

Uint32 enemyGetAttackTime(Enemies aEnemies, int enemyindex);

void enemySetAttackTime(Enemies aEnemies, int enemyindex);

SDL_Rect enemyGetRect(Enemies aEnemies, int index);

void enemyDamaged(Enemies aEnemies, int damage, int index, int *enemyCount);

int enemyGetCount(Enemies aEnemies);

bool enemyColitino(SDL_Rect A,SDL_Rect B);


#endif