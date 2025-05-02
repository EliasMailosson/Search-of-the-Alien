#ifndef ENEMIES_H
#define ENEMIES_H

#include "../../include/NET/shared.h"
#include "../../include/NET/serverLogic.h"

#define MAX_ENEMIES 3

typedef struct enemies* Enemies;
typedef struct enemy Enemy;
typedef struct ServerMap *ServerMap;
// enum id?
enum enemyID {
    LIGHT_ENEMY, 
    HEAVY_ENEMY, 
    BOSS_ENEMY
};

Enemies enemyCreate();

SDL_Point enemyGetPoint(Enemies aEnemies, int index);

void checkEnemyCollision(Enemies aEnemies, int enemyindex, int *collide);
SDL_Rect enemyGetHitbox(Enemies aEnemies, int index);
// int GetEnemyCount(Enemies aEnemies);
void SetEnemyHitbox(Enemies aEnemies, int enemyindex, SDL_Rect H);

void enemySpawn(Enemies aEnemies);
void enemyAI(Enemies aEnemies, SDL_Point playerpos); //ServerMap aMap

void PlayerTracker(Enemies aEnemies, SDL_Point playerPos, int enemyindex); //ServerMap aMap

void enemyDestroy(Enemies aEnemies);


#endif