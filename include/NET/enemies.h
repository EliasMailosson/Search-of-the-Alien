#ifndef ENEMIES_H
#define ENEMIES_H

#include "../../include/NET/shared.h"

typedef struct healthPoints HealthPoints;

typedef struct enemy Enemy;

typedef struct enemies *Enemies;

// enum id?
enum enemyID {LIGHT_ENEMY, HEAVY_ENEMY, BOSS_ENEMY};

Enemies enemyCreate();

SDL_Point enemyGetPoint(Enemies aEnemies, int index);

void enemySpawn(Enemies aEnemies);
enemyClosestPlayer(Uint32 TimeLeft);
void enemyUpdatePos(Enemies aEnemies, SDL_Point playerPos);

void enemyDestroy(Enemies aEnemies);


#endif