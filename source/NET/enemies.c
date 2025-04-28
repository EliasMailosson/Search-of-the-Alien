#include "../../include/NET/enemies.h"

struct enemy{
    SDL_Rect enemyRect;
    int enemyID;

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
    for (int i = 0; i < MAX_ENEMIES; i++)
    {   
        aEnemies->enemyList->enemyRect.x = i*5;
        aEnemies->enemyList->enemyRect.y = i*5;
    }
}

void enemyUpdatePos(Enemies aEnemies, SDL_Point playerPos){
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        int enemyX = aEnemies->enemyList[i].enemyRect.x;
        int enemyY = aEnemies->enemyList[i].enemyRect.y;

        if(enemyX > playerPos.x) enemyX--;

        if(enemyX < playerPos.x) enemyX++;

        if(enemyY > playerPos.y) enemyY--;

        if(enemyY < playerPos.y) enemyY++;

        aEnemies->enemyList[i].enemyRect.x = enemyX;
        aEnemies->enemyList[i].enemyRect.y = enemyY;
    }

}

SDL_Rect enemyGetRect(Enemies aEnemies, int index){
    return aEnemies->enemyList[index].enemyRect;
}

void enemyDestroy(Enemies aEnemies){
    free(aEnemies);
    aEnemies = NULL;
}