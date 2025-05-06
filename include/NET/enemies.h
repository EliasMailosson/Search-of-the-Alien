#ifndef ENEMIES_H
#define ENEMIES_H
#include "../../include/NET/shared.h"
#include "serverLogic.h"

#define MAX_ENEMIES 3

typedef struct ServerMap *ServerMap;
typedef struct enemies* Enemies;
typedef struct enemy Enemy;
// enum id?
enum enemyID {LIGHT_ENEMY, HEAVY_ENEMY, BOSS_ENEMY};

Enemies enemyCreate();

SDL_Point enemyGetPoint(Enemies aEnemies, int index);

void enemySpawn(Enemies aEnemies, ServerMap aServerMap);

void enemyAI(Enemies aEnemies, SDL_Point playerpos);

void PlayerTracker(Enemies aEnemies, SDL_Point playerPos, int enemyindex);

void enemyAngleTracker(Enemies aEnemies, SDL_Point playerPos, int enemyIndex);

int enemyGetDirection(Enemies aEnemies, int index);

float enemyGetAngle(Enemies aEnemies, int index);

void enemyDestroy(Enemies aEnemies);

SDL_Rect enemyGetRect(Enemies aEnemies, int index);

void enemyDamaged(Enemies aEnemies, int damage, int index, int *enemyCount);

int enemyGetCount(Enemies aEnemies);

bool enemyColitino(SDL_Rect A,SDL_Rect B);

#endif