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

Enemies enemyCreate(){
    Enemies aEnemies =  malloc(sizeof(struct enemies));
    aEnemies->count = 1;
    if(aEnemies == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }
    aEnemies->enemyList = malloc(sizeof(struct enemy) * aEnemies->count);
    if(aEnemies->enemyList == NULL){
        free(aEnemies);
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }

    return aEnemies;
}

void enemySpawn(Enemies aEnemies){
    for (int i = 0; i < aEnemies->count; i++){
        aEnemies->enemyList[i].enemyRect.x = i*64;
        aEnemies->enemyList[i].enemyRect.y = i*512;
        aEnemies->enemyList[i].enemyRect.w = 64;
        aEnemies->enemyList[i].enemyRect.h = 64;
        aEnemies->enemyList[i].ThinkTime = 0;
        aEnemies->enemyList[i].direction = 0;
        aEnemies->enemyList[i].angle = 0.0f;
        aEnemies->enemyList[i].HP.maxHP = 100;
        aEnemies->enemyList[i].HP.currentHP = 100;
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
    int dx = 0;
    int dy = 0;

    Enemy *enemy = &aEnemies->enemyList[enemyindex];

    if (enemy->enemyRect.x < playerPos.x + 64)      dx =  speed; // Hard coded + 64 to get middle of the Player
    else if (enemy->enemyRect.x > playerPos.x + 64) dx = -speed;

    if (enemy->enemyRect.y < playerPos.y + 64)      dy =  speed;
    else if (enemy->enemyRect.y > playerPos.y + 64) dy = -speed;

    enemy->enemyRect.x += dx;
    enemy->enemyRect.y += dy;
}

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

SDL_Rect enemyGetRect(Enemies aEnemies, int index){
    return aEnemies->enemyList[index].enemyRect;
}

void enemyDamaged(Enemies aEmemies, int damage, int index, int *pEnemyCount){
    aEmemies->enemyList[index].HP.currentHP -= damage;
    printf("%f\n", aEmemies->enemyList[index].HP.currentHP);
    if (aEmemies->enemyList[index].HP.currentHP <= 0)
    {
        *pEnemyCount -= 1;
    }
}

int enemyGetCount(Enemies aEnemies){
    return aEnemies->count;
}

bool enemyColitino(SDL_Rect A,SDL_Rect B){
    if(A.x + A.w >= B.x && 
        A.x <= B.x + B.w &&
        A.y + A.h >= B.y &&
        A.y <= B.y + B.h){
        return true;
    }else{
        return false;
    }
}