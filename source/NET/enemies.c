#include "../../include/NET/enemies.h"


struct healthPoints{
    float maxHP;
    float currentHP;
};
struct enemy{
    SDL_Rect enemyRect;
    int enemyID;
    HealthPoints HP;
};
struct enemies{
    Enemy enemyList[MAX_ENEMIES];
};

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

    aEnemies->enemyList[1].enemyRect.y = 256;
    aEnemies->enemyList[1].enemyRect.y = 256;

    aEnemies->enemyList[2].enemyRect.y = 512;
    aEnemies->enemyList[2].enemyRect.y = 512;

}

void enemyUpdatePos(Enemies aEnemies, SDL_Point playerPos){

    const int speed = 1;
    //printf("Enemy-pos: %d %d, Player-pos: %d %d\n", aEnemies->enemyList[0].enemyRect.x, aEnemies->enemyList[0].enemyRect.y, playerPos.x, playerPos.y);

    for (int i = 0; i < MAX_ENEMIES; ++i)
    {
        if (aEnemies->enemyList[i].enemyRect.x < playerPos.x)
            aEnemies->enemyList[i].enemyRect.x += speed;
        else if (aEnemies->enemyList[i].enemyRect.x > playerPos.x)
            aEnemies->enemyList[i].enemyRect.x -= speed;

        if (aEnemies->enemyList[i].enemyRect.y < playerPos.y)
            aEnemies->enemyList[i].enemyRect.y += speed;
        else if (aEnemies->enemyList[i].enemyRect.y > playerPos.y)
            aEnemies->enemyList[i].enemyRect.y -= speed;
    }
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