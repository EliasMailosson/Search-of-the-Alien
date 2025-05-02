#ifndef ENEMIES_H
#define ENEMIES_H

#include "../../include/NET/shared.h"
#include "../../include/NET/serverLogic.h"

#define MAX_ENEMIES 5

typedef struct enemies* Enemies;
typedef struct enemy Enemy;
typedef struct ServerMap *ServerMap;

enum enemyID {
    LIGHT_ENEMY, 
    HEAVY_ENEMY, 
    BOSS_ENEMY
};

Enemies enemyCreate(int capacity);

SDL_Point enemyGetPoint(Enemies aEnemies, int index);

void checkEnemyCollision(Enemies aEnemies, int enemyindex, int *collide);
SDL_Rect enemyGetHitbox(Enemies aEnemies, int index);
// int GetEnemyCount(Enemies aEnemies);
void SetEnemyHitbox(Enemies aEnemies, int enemyindex, SDL_Rect H);

void enemySpawn(Enemies aEnemies);

void PlayerTracker(Enemies aEnemies, SDL_Point playerPos, int enemyindex, ServerMap aMap); //ServerMap aMap

void enemyAngleTracker(Enemies aEnemies, SDL_Point playerPos, int enemyIndex);

int enemyGetDirection(Enemies aEnemies, int index);

float enemyGetAngle(Enemies aEnemies, int index);

void enemyDestroy(Enemies aEnemies);


#endif