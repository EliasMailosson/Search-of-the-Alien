#ifndef ENEMIES_H
#define ENEMIES_H
#include "../../include/NET/shared.h"

#define MAX_ENEMIES 5

typedef struct enemies* Enemies;
typedef struct enemy Enemy;
// enum id?
enum enemyID {LIGHT_ENEMY, HEAVY_ENEMY, BOSS_ENEMY};

Enemies enemyCreate();

SDL_Point enemyGetPoint(Enemies aEnemies, int index);

void enemySpawn(Enemies aEnemies);

void enemyAI(Enemies aEnemies, SDL_Point playerpos);

void PlayerTracker(Enemies aEnemies, SDL_Point playerPos, int enemyindex);

void enemyAngleTracker(Enemies aEnemies, SDL_Point playerPos, int enemyIndex);

int enemyGetDirection(Enemies aEnemies, int index);

float enemyGetAngle(Enemies aEnemies, int index);

void enemyDestroy(Enemies aEnemies);


#endif