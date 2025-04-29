#ifndef ENEMIES_H
#define ENEMIES_H

#include "../../include/NET/shared.h"
#define MAX_ENEMIES 3

typedef struct healthPoints {
    float maxHP;
    float currentHP;
} HealthPoints;

typedef struct enemy {
    SDL_Rect enemyRect;
    int enemyID;
    HealthPoints HP;
    Uint32 ThinkTime;
} Enemy;

typedef struct enemies {
    Enemy enemyList[MAX_ENEMIES];
} *Enemies;

// enum id?
enum enemyID {LIGHT_ENEMY, HEAVY_ENEMY, BOSS_ENEMY};

Enemies enemyCreate();

SDL_Point enemyGetPoint(Enemies aEnemies, int index);

void enemySpawn(Enemies aEnemies);

void PlayerTracker(Enemies aEnemies, SDL_Point playerPos, int enemyindex);

void enemyDestroy(Enemies aEnemies);


#endif