#include "../../include/NET/enemies.h"

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
}enemies;

Enemies enemyCreate(){
    Enemies aEnemies =  malloc(sizeof(struct enemies));
    if(aEnemies == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }

    return aEnemies;
}

void enemySpawn(Enemies aEnemies){
    aEnemies->enemyList[0].enemyRect.x = 128;
    aEnemies->enemyList[0].enemyRect.y = 128;

    aEnemies->enemyList[1].enemyRect.x = 256;
    aEnemies->enemyList[1].enemyRect.y = 256;

    aEnemies->enemyList[2].enemyRect.x = 512;
    aEnemies->enemyList[2].enemyRect.y = 512;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        aEnemies->enemyList[i].ThinkTime = 0;
    }
}

void enemyAI(Enemies aEnemies, SDL_Point playerpos){

    Uint32 CurrentThinkTime = SDL_GetTicks();
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (CurrentThinkTime >= aEnemies->enemyList[i].ThinkTime)
        {
            PlayerTracker(aEnemies, playerpos, i);
            aEnemies->enemyList[i].ThinkTime = CurrentThinkTime + 3000;
        }
    }
}

void PlayerTracker(Enemies aEnemies, SDL_Point playerPos, int enemyindex){

    const int speed = 1;
    //printf("Enemy-pos: %d %d, Player-pos: %d %d\n", aEnemies->enemyList[0].enemyRect.x, aEnemies->enemyList[0].enemyRect.y, playerPos.x, playerPos.y);

    Enemy* enemy = &aEnemies->enemyList[enemyindex];
    
        if (enemy->enemyRect.x < playerPos.x)
            enemy->enemyRect.x += speed;
        else if (enemy->enemyRect.x > playerPos.x)
            enemy->enemyRect.x -= speed;

        if (enemy->enemyRect.y < playerPos.y)
            enemy->enemyRect.y += speed;
        else if (enemy->enemyRect.y > playerPos.y)
            enemy->enemyRect.y -= speed;
}

SDL_Point enemyGetPoint(Enemies aEnemies, int index){
    SDL_Point point = {
        .x = aEnemies->enemyList[index].enemyRect.x,
        .y = aEnemies->enemyList[index].enemyRect.y
    };
    return point;
}

void enemyDestroy(Enemies aEnemies){
    free(aEnemies);
    aEnemies = NULL;
}