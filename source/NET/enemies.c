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
    int direction;
    float angle;
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
    for (int i = 0; i < MAX_ENEMIES; i++){
        aEnemies->enemyList[i].enemyRect.x = i*64;
        aEnemies->enemyList[i].enemyRect.y = i*64;
        aEnemies->enemyList[i].ThinkTime = 0;
        aEnemies->enemyList[i].direction = 0;
        aEnemies->enemyList[i].angle = 0.0f;
    }
}

void enemyAI(Enemies aEnemies, SDL_Point playerpos){

    Uint32 CurrentThinkTime = SDL_GetTicks();
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (CurrentThinkTime >= aEnemies->enemyList[i].ThinkTime)
        {
            PlayerTracker(aEnemies, playerpos, i);
            enemyAngleTracker(aEnemies, playerpos, i);
            aEnemies->enemyList[i].ThinkTime = CurrentThinkTime + 30;
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

void enemyAngleTracker(Enemies aEnemies, SDL_Point playerPos, int enemyIndex){

    SDL_Rect r  = aEnemies->enemyList[enemyIndex].enemyRect;
    float dx = (float)playerPos.x - (float)r.x;
    float dy = (float)playerPos.y - (float)r.y;

    // if you’re on top of the player, skip
    float dist = sqrtf(dx*dx + dy*dy);
    
    // normalize
    //dx /= dist;  
    //dy /= dist;

    // move
    //r->x += (int)roundf(dx * speed);
    //r->y += (int)roundf(dy * speed);

    // compute angle & update your direction enum
    float angle;
    aEnemies->enemyList[enemyIndex].angle = angle = atan2f(dy, dx);
    aEnemies->enemyList[enemyIndex].direction = ((int)roundf(angle / (float)M_PI_4) + 7 + 4 ) % 8;

    printf("server #%d: %d\n", enemyIndex, aEnemies->enemyList[enemyIndex].direction);
}

int enemyGetDirection(Enemies aEnemies, int index){
    /*int* direction = &aEnemies->enemyList[index].direction;
    float* angle = &aEnemies->enemyList[index].angle;
    *direction = ((int)roundf((*angle) / (float)M_PI_4) + 7 ) % 8;*/
    //float angle = aEnemies->enemyList[index].angle;
    //int dir = ((int)roundf(angle / (float)M_PI_4));
    //aEnemies->enemyList[index].direction = (dir + 7) % 8;
    return aEnemies->enemyList[index].direction;
}

float enemyGetAngle(Enemies aEnemies, int index){
    return aEnemies->enemyList[index].angle;
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