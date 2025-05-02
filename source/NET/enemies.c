#include "../../include/NET/enemies.h"

typedef struct healthPoints {
    float maxHP;
    float currentHP;
} HealthPoints;

typedef struct enemy {
    SDL_Rect hitbox;
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
    aEnemies->enemyList[0].hitbox.x = 250;
    aEnemies->enemyList[0].hitbox.y = 1000;
    
    aEnemies->enemyList[1].hitbox.x = 200;
    aEnemies->enemyList[1].hitbox.y = 1000;

    aEnemies->enemyList[2].hitbox.x = 300;
    aEnemies->enemyList[2].hitbox.y = 1000;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        aEnemies->enemyList[i].ThinkTime = 0;
        aEnemies->enemyList[i].hitbox.w = 40;
        aEnemies->enemyList[i].hitbox.h = 40;
    }
}

void PlayerTracker(Enemies aEnemies, SDL_Point playerPos, int enemyindex, ServerMap aMap){

    const int speed = 1;

    Enemy* enemy = &aEnemies->enemyList[enemyindex];
    SDL_Rect oldPos = enemy->hitbox;
    
        if (enemy->hitbox.x < playerPos.x)
            enemy->hitbox.x += speed;
        else if (enemy->hitbox.x > playerPos.x)
            enemy->hitbox.x -= speed;

        if (enemy->hitbox.y < playerPos.y)
            enemy->hitbox.y += speed;
        else if (enemy->hitbox.y > playerPos.y)
            enemy->hitbox.y -= speed;

        if (MAP_TileNotWalkable(aMap, enemy->hitbox.x, enemy->hitbox.y)) {
            enemy->hitbox = oldPos;
            printf("enemy not walk lol\n");
            return;
        }

        int collision = 0;
        checkEnemyCollision(aEnemies,enemyindex,&collision);

        SDL_Rect playerHitbox = { playerPos.x, playerPos.y, enemy->hitbox.w, enemy->hitbox.h };
        bool collidedWithPlayer = SDL_HasIntersection(&enemy->hitbox, &playerHitbox);

        if (collision || collidedWithPlayer)
        {
            enemy->hitbox = oldPos;
        }
}

void checkEnemyCollision(Enemies aEnemies, int enemyindex, int *collide){
    int enemyCount = MAX_ENEMIES;
    float resistance = 1.0f;

    *collide = 0;
    for (int i = 0; i < enemyCount; i++)
    {
        for (int j = i + 1; j < enemyCount; j++)
        {
            SDL_Rect e1 = enemyGetHitbox(aEnemies,i);
            SDL_Rect e2 = enemyGetHitbox(aEnemies,j);

            float e1CenterX = e1.x + e1.w / 2.0f;
            float e1CenterY = e1.y + e1.h / 2.0f;
            float e2CenterX = e2.x + e2.w / 2.0f;
            float e2CenterY = e2.y + e2.h / 2.0f;

            bool isIntersected = (
                e1.x < e2.x + e2.w &&
                e1.x + e1.w > e2.x &&
                e1.y < e2.y + e2.h &&
                e1.y + e1.h > e2.y
            );

            if (isIntersected)
            {
                float intersectX = (e1.w / 2.0f + e2.w / 2.0f) - fabs(e1CenterX - e2CenterX);
                float intersectY = (e1.h / 2.0f + e2.h / 2.0f) - fabs(e1CenterY - e2CenterY);

                if (intersectX < intersectY)
                {
                    if ((enemyindex == i || enemyindex == j) && *collide == 0)
                    {
                        *collide = 1;
                    }
                    if (e1CenterX < e2CenterX){
                        e1.x -= (int)ceil(resistance);
                        e2.x += (int)ceil(resistance);
                    }else{
                        e1.x += (int)ceil(resistance);
                        e2.x -= (int)ceil(resistance);
                    } 
                } else {
                    if ((enemyindex == i || enemyindex == j) && *collide == 0)
                    {
                        *collide = 1;
                    }
                    if (e1CenterY < e2CenterY)
                    {
                        e1.y -= (int)ceil(resistance);
                        e2.y += (int)ceil(resistance);
                    } else {
                        e1.y += (int)ceil(resistance);
                        e2.y -= (int)ceil(resistance);
                    }
                }
            }
        }
    }
}

void SetEnemyHitbox(Enemies aEnemies, int enemyindex, SDL_Rect HB){
    aEnemies->enemyList[enemyindex].hitbox = HB;
}

SDL_Rect enemyGetHitbox(Enemies aEnemies, int index){
    SDL_Rect hitbox = {
        .x = aEnemies->enemyList[index].hitbox.x,
        .y = aEnemies->enemyList[index].hitbox.y,
        .w = aEnemies->enemyList[index].hitbox.w,
        .h = aEnemies->enemyList[index].hitbox.h
    };
    return hitbox;
}

// int GetEnemyCount(Enemies aEnemies){
//     aEnemies = (sizeof(aEnemies->enemyList)/sizeof(Enemies));
//     return aEnemies->enemyList;
// }

SDL_Point enemyGetPoint(Enemies aEnemies, int index){
    SDL_Point point = {
        .x = aEnemies->enemyList[index].hitbox.x,
        .y = aEnemies->enemyList[index].hitbox.y
    };

    return point;
}

void enemyDestroy(Enemies aEnemies){
    free(aEnemies);
    aEnemies = NULL;
}