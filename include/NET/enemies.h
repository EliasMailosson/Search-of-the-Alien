#ifndef ENEMIES_H
#define ENEMIES_H
#include "../../include/NET/shared.h"

#define MAX_ENEMIES 3

typedef struct enemies* Enemies;
typedef struct enemy Enemy;
// enum id?
enum enemyID {LIGHT_ENEMY, HEAVY_ENEMY, BOSS_ENEMY};

Enemies enemyCreate();

SDL_Point enemyGetPoint(Enemies aEnemies, int index);

void enemySpawn(Enemies aEnemies);

void PlayerTracker(Enemies aEnemies, SDL_Point playerPos, int enemyindex);

void enemyDestroy(Enemies aEnemies);


#endif