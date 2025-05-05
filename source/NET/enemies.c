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
	aEnemies->count = capacity;

	return aEnemies;
}

void enemySpawn(Enemies aEnemies){
	for (int i = 0; i < MAX_ENEMIES; i++){
		aEnemies->enemyList[i].hitbox.x = i*64;
		aEnemies->enemyList[i].hitbox.y = i*512;
		aEnemies->enemyList[i].hitbox.w = 50;
		aEnemies->enemyList[i].hitbox.h = 50;
		aEnemies->enemyList[i].ThinkTime = 0;
		aEnemies->enemyList[i].direction = 0;
		aEnemies->enemyList[i].angle = 0.0f;
		aEnemies->enemyList[i].HP.maxHP = 100;
		aEnemies->enemyList[i].HP.currentHP = 100;
	}
}

void PlayerTracker(Enemies aEnemies, Server aServer, int playerIndex, int enemyIndex, ServerMap aMap) {
    const float speed       = 1.0f;
    const float seekWeight  = 1.0f;
    const float sepWeight   = 3.0f;// styr hur stark separationen är
    const float sepRadius   = 64.0f;//hur nära en fiende ska va för att den ska börja knuffa dem
    
    Enemy* e = &aEnemies->enemyList[enemyIndex];
    SDL_Rect old = e->hitbox;
    
    //Hämta seek‐vektor mot spelaren
    SDL_Rect playerHB = NET_serverGetPlayerHitbox(aServer, playerIndex);
    float ex = e->hitbox.x + e->hitbox.w/2.0f;
    float ey = e->hitbox.y + e->hitbox.h/2.0f;
    float px = playerHB.x   + playerHB.w/2.0f;
    float py = playerHB.y   + playerHB.h/2.0f;
    
    float vx = (px - ex) * seekWeight;
    float vy = (py - ey) * seekWeight;
    
    //Separation från alla andra fiender, "o" står för "e2" egentligen
    for (int j = 0; j < aEnemies->count; j++) {
        if (j == enemyIndex) continue;

        Enemy* o = &aEnemies->enemyList[j];
        float ox = o->hitbox.x + o->hitbox.w/2.0f;
        float oy = o->hitbox.y + o->hitbox.h/2.0f;
        
        float dx = ex - ox;
        float dy = ey - oy;
        float dist2 = dx*dx + dy*dy;
        
        if (dist2 > 0 && dist2 < sepRadius*sepRadius) {
            //Ju närmare, desto starkare knuff
            float dist = sqrtf(dist2);
			float force = (sepRadius - dist) / sepRadius;
            vx += (dx * dist) * force * sepWeight;
            vy += (dy * dist) * force * sepWeight;
        }
    }
    
    //Normalisera vektorn och applicera speed
    float mag = sqrtf(vx*vx + vy*vy);
    if (mag > 0.0f) {
        vx = vx / mag * speed;
        vy = vy / mag * speed;
        
        e->hitbox.x = (int)roundf(e->hitbox.x + vx);
        e->hitbox.y = (int)roundf(e->hitbox.y + vy);
    }
    
    //Kontroll mot kartan/tiles
    // if (e->hitbox.x < 0 || e->hitbox.y < 0 ||
    //     e->hitbox.x + e->hitbox.w > MAP_WIDTH ||
    //     e->hitbox.y + e->hitbox.h > MAP_HEIGHT ||
    //     MAP_TileNotWalkable(aMap, e->hitbox.x, e->hitbox.y))
    // {
    //     e->hitbox = old;
    // }
    
    //Kollision med spelaren(stannar precis vid spelarens rekt)
    if (SDL_HasIntersection(&e->hitbox, &playerHB)) {
        e->hitbox = old;
    }
}

void checkEnemyCollision(Enemies aEnemies, int enemyindex, int *collide){
	int enemyCount = MAX_ENEMIES;
	Enemy *movingEnemy = &aEnemies->enemyList[enemyindex];
	SDL_Rect *movingHitbox = &movingEnemy->hitbox;

	*collide = 0;
	for (int i = 0; i < enemyCount; i++) {
		if (i == enemyindex) continue; 
	
		Enemy *otherEnemy = &aEnemies->enemyList[i];
		SDL_Rect *otherHitbox = &otherEnemy->hitbox;
	
		if (SDL_HasIntersection(movingHitbox, otherHitbox)) {
			*collide = 1;
				return; //Räcker att veta att vi krockar, rörelsen ska ändå brytas
			}
	}
}


void enemyAngleTracker(Enemies aEnemies, SDL_Point playerPos, int enemyIndex) {
	Enemy *enemy = &aEnemies->enemyList[enemyIndex];
	playerPos.x+=64;
	playerPos.y+=64;

	float dx = playerPos.x - (float)enemy->hitbox.x;
	float dy = playerPos.y - (float)enemy->hitbox.y;
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

void SetEnemyHitbox(Enemies aEnemies, int enemyindex, SDL_Rect HB){
	aEnemies->enemyList[enemyindex].hitbox = HB;
}

// SDL_Rect enemyGetHitbox(Enemies aEnemies, int index){
// 	SDL_Rect hitbox = {
// 		.x = aEnemies->enemyList[index].hitbox.x,
// 		.y = aEnemies->enemyList[index].hitbox.y,
// 		.w = aEnemies->enemyList[index].hitbox.w,
// 		.h = aEnemies->enemyList[index].hitbox.h
// 	};
// 	return hitbox;
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