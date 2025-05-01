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
    Enemy *enemyList;
    int count;
    int capacity;
}enemies;

Enemies enemyCreate(int capacity){
    Enemies aEnemies =  malloc(sizeof(struct enemies));
    if(aEnemies == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }
    aEnemies->enemyList = malloc(sizeof(struct enemy) * capacity);
    if(aEnemies->enemyList == NULL){
        free(aEnemies);
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }

    return aEnemies;
}

void enemySpawn(Enemies aEnemies){
    for (int i = 0; i < MAX_ENEMIES; i++){
        aEnemies->enemyList[i].enemyRect.x = i*64;
        aEnemies->enemyList[i].enemyRect.y = i*512;
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
    const int speed = 2;
    int dx = 0;
    int dy = 0;

    Enemy *enemy = &aEnemies->enemyList[enemyindex];

    if (enemy->enemyRect.x < playerPos.x + 64)      dx =  speed;
    else if (enemy->enemyRect.x > playerPos.x + 64) dx = -speed;

    if (enemy->enemyRect.y < playerPos.y + 64)      dy =  speed;
    else if (enemy->enemyRect.y > playerPos.y + 64) dy = -speed;

    enemy->enemyRect.x += dx;
    enemy->enemyRect.y += dy;
}

/*void PlayerTracker(Enemies aEnemies, SDL_Point playerPos, int enemyindex){
    const int speed = 2;
    int dx = 0;
    int dy = 0;

    Enemy *enemy = &aEnemies->enemyList[enemyindex];

    if (enemy->enemyRect.x < playerPos.x + 64)      dx =  speed;
    else if (enemy->enemyRect.x > playerPos.x + 64) dx = -speed;

    if (enemy->enemyRect.y < playerPos.y + 64)      dy =  speed;
    else if (enemy->enemyRect.y > playerPos.y + 64) dy = -speed;

    if (dx && dy ) {
        dy = dy / 2;   
    }

    enemy->enemyRect.x += dx;
    enemy->enemyRect.y += dy;
}*/

/*void enemyAngleTracker(Enemies aEnemies, SDL_Point playerPos, int enemyIndex){

    SDL_Rect r  = aEnemies->enemyList[enemyIndex].enemyRect;

    playerPos.x+=64;
    playerPos.y+=64;


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
    aEnemies->enemyList[enemyIndex].direction = ((int)roundf(angle / (float)M_PI_4) + 7 + 4 ) % 8; // + 7 + 4

    //printf("server #%d: %d\n", enemyIndex, aEnemies->enemyList[enemyIndex].direction);
}*/

void enemyAngleTracker(Enemies aEnemies, SDL_Point playerPos, int enemyIndex) {
    Enemy *enemy = &aEnemies->enemyList[enemyIndex];
    playerPos.x+=64;
    playerPos.y+=64;

    float dx = playerPos.x - (float)enemy->enemyRect.x;
    float dy = playerPos.y - (float)enemy->enemyRect.y;
    float angle = atan2f(dy, dx);   
    enemy->angle = angle;

    int raw = (int)roundf(angle / M_PI_4);  

    enemy->direction = (raw + 3 + 8) % 8;         
}


int enemyGetDirection(Enemies aEnemies, int index){
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