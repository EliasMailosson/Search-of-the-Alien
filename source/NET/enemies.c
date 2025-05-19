#include "../../include/NET/enemies.h"

typedef struct healthPoints {
	float maxHP;
	float currentHP;
} HealthPoints;

struct enemy{
	SDL_Rect hitbox;
	int enemyID;
	HealthPoints HP;
	Uint32 ThinkTime;
	int direction;
	float angle;
    Uint32 attackTime;
};

typedef struct enemies {
	Enemy *enemyList;
	size_t size;
	size_t capacity;
    SDL_Rect *zones;
    int zoneCount;
}enemies;
    
Enemies NET_enemiesCreate(void){
	Enemies aEnemies =  malloc(sizeof(*aEnemies));
	if(aEnemies == NULL){
		fprintf(stderr,"Error allocating memory for server\n");
		return NULL;
	}
    aEnemies->enemyList = NULL;
	aEnemies->size = 0;
    aEnemies->capacity = 0;
	return aEnemies;
}

int NET_enemiesPush(Enemies aE, Enemy new){
    if (aE->size == aE->capacity) {
        size_t new_cap = aE->capacity ? aE->capacity * 2 : 4;
        Enemy *tmp = realloc(aE->enemyList, new_cap * sizeof *tmp);
        if (!tmp) return -1;
        aE->enemyList     = tmp;
        aE->capacity = new_cap;
    }
    aE->enemyList[aE->size++] = new;
    //printf("new count e %d\n",(int)aE->size);
    return 0;
}

void NET_enemiesClear(Enemies aEnemies){
    aEnemies->size = 0;
}

Enemy NET_enemiesPopAt(Enemies aE, size_t index){
    if (!aE || index >= aE->size) {
        return NULL;
    }
    Enemy popped = aE->enemyList[index];
    for (size_t i = index; i < aE->size - 1; ++i) {
        aE->enemyList[i] = aE->enemyList[i + 1];
    }
    aE->enemyList[aE->size - 1] = NULL;
    aE->size--;
    return popped;
}

SDL_Rect NET_getEnemySpawnZone(SDL_Rect playerRect, int tile) {
    tile = 1;
	SDL_Rect zone;
    int paddingPixels = tile * TILE_SIZE; //hur många "tile" bort, TILE_SIZE konverterar det till en pixel mått

    zone.x = playerRect.x - paddingPixels;
    zone.y = playerRect.y - paddingPixels;
    zone.w = playerRect.w + 2 * paddingPixels;
    zone.h = playerRect.h + 2 * paddingPixels;

	// printf("SpawnZone around player: x=%d y=%d w=%d h=%d\n", zone.x, zone.y, zone.w, zone.h);

    return zone;
}

Enemy NET_enemyCreate(int pixelX, int pixelY, EnemyID id, const int difficulty){
    Enemy e = malloc(sizeof(*e));
    if (!e) {
        fprintf(stderr, "Failed to allocate memory for Enemy\n");
        return NULL;
    }
    e->hitbox.y = pixelY;
    e->hitbox.x = pixelX;
    switch (id){
    case LIGHT_ENEMY:
        e->hitbox.h = e->hitbox.w = 50;
        e->ThinkTime = e->direction = e->attackTime = 0;
        e->HP.maxHP = e->HP.currentHP = 100 * difficulty;
        e->enemyID = LIGHT_ENEMY;
        break;
    case HEAVY_ENEMY:
        break;
    case BOSS_ENEMY:
        break;
    default:  
        printf("Wrong enemy id in file enemies.c\n");
        break;
    }
    return e;
}

Enemy NET_enemiesGetAt(Enemies aE, size_t index){
    if (!aE || index >= aE->size) {
        return NULL;
    }
    return aE->enemyList[index];
}

void PlayerTracker(Enemies aEnemies, Server aServer, int playerIndex, int enemyIndex) {
    if (!aEnemies || enemyIndex < 0 || (size_t)enemyIndex >= aEnemies->size || aEnemies->enemyList[enemyIndex] == NULL){
        //fprintf(stderr,"enemyGetAttackTime: Invalid index %d (size: %zu)\n", enemyIndex, aEnemies ? aEnemies->size : 0);
        return;
    }
    if (!aServer || playerIndex < 0){
        //fprintf(stderr, "PlayerTracker: invalid playerIndex %d (clientCount: %d)\n", playerIndex, aServer ? NET_serverGetClientCount(aServer) : 0);
        return;
    }
    const float speed       = 1.0f;
    const float seekWeight  = 1.0f;
    const float sepWeight   = 3.0f;// styr hur stark separationen är
    const float sepRadius   = 64.0f;//hur nära en fiende ska va för att den ska börja knuffa dem
    
    Enemy e = aEnemies->enemyList[enemyIndex];
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
    for (size_t j = 0; j < aEnemies->size; j++) {
        if (j == (size_t)enemyIndex) continue;

        Enemy o = aEnemies->enemyList[j];
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
	Enemy movingEnemy = aEnemies->enemyList[enemyindex];
	SDL_Rect *movingHitbox = &movingEnemy->hitbox;

	*collide = 0;
	for (int i = 0; i < (int)aEnemies->size; i++) {
		if (i == enemyindex) continue; 
	
		Enemy otherEnemy = aEnemies->enemyList[i];
		SDL_Rect *otherHitbox = &otherEnemy->hitbox;
	
		if (SDL_HasIntersection(movingHitbox, otherHitbox)) {
			*collide = 1;
				return; //Räcker att veta att vi krockar, rörelsen ska ändå brytas
			}
	}
}


void enemyAngleTracker(Enemies aEnemies, SDL_Point playerPos, int enemyIndex) {
    if (!aEnemies || enemyIndex < 0 || (size_t)enemyIndex >= aEnemies->size || aEnemies->enemyList[enemyIndex] == NULL){
        //fprintf(stderr,"enemyGetAttackTime: Invalid index %d (size: %zu)\n", enemyIndex, aEnemies ? aEnemies->size : 0);
        return;
    }
	Enemy enemy = aEnemies->enemyList[enemyIndex];
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
    if (!aEnemies || index >= (int)aEnemies->size) {
        return 0;
    }
	return aEnemies->enemyList[index]->direction;
}

float enemyGetAngle(Enemies aEnemies, int index){
    if (!aEnemies || index >= (int)aEnemies->size) {
        return 0.0f;
    }
	return aEnemies->enemyList[index]->angle;
}

void SetEnemyHitbox(Enemies aEnemies, int enemyindex, SDL_Rect HB){
    if (!aEnemies || enemyindex >= (int)aEnemies->size) {
        return;
    }
	aEnemies->enemyList[enemyindex]->hitbox = HB;
}

Uint32 enemyGetAttackTime(Enemies aEnemies, int enemyindex){
    if (!aEnemies || enemyindex < 0 || (size_t)enemyindex >= aEnemies->size || aEnemies->enemyList[enemyindex] == NULL){
        //fprintf(stderr,"enemyGetAttackTime: Invalid index %d (size: %zu)\n", enemyindex, aEnemies ? aEnemies->size : 0);
        return 0;
    }
	return aEnemies->enemyList[enemyindex]->attackTime;
}

void enemySetAttackTime(Enemies aEnemies, int enemyindex){
    if (!aEnemies || enemyindex < 0 || (size_t)enemyindex >= aEnemies->size || aEnemies->enemyList[enemyindex] == NULL){
        //fprintf(stderr,"enemySetAttackTime: Invalid index %d (size: %zu)\n", enemyindex, aEnemies ? aEnemies->size : 0);
        return;
    }
	aEnemies->enemyList[enemyindex]->attackTime = SDL_GetTicks();
}

SDL_Rect enemyGetHitbox(Enemies aEnemies, int index){
    if (!aEnemies || index < 0 || (size_t)index >= aEnemies->size || aEnemies->enemyList[index] == NULL){
        //fprintf(stderr,"enemyGetHitbox: Invalid index %d (size: %zu)\n", index, aEnemies ? aEnemies->size : 0);
        return (SDL_Rect){0, 0, 0, 0};
    }
    SDL_Rect hitbox = {
        .x = aEnemies->enemyList[index]->hitbox.x,
        .y = aEnemies->enemyList[index]->hitbox.y,
        .w = aEnemies->enemyList[index]->hitbox.w,
        .h = aEnemies->enemyList[index]->hitbox.h
    };
    return hitbox;
}

SDL_Point enemyGetPoint(Enemies aEnemies, int index) {
    if (!aEnemies || index < 0 || (size_t)index >= aEnemies->size) {
        //fprintf(stderr, "enemyGetPoint: Invalid index %d (size: %zu)\n", index, aEnemies ? aEnemies->size : 0);
        return (SDL_Point){0, 0}; // or handle differently
    }
    return (SDL_Point){
        .x = aEnemies->enemyList[index]->hitbox.x,
        .y = aEnemies->enemyList[index]->hitbox.y};
}

void NET_enemiesDestroy(Enemies aEnemies){
    for (size_t i = 0; i < aEnemies->size; i++){
        if(aEnemies->enemyList[i] != NULL) free(aEnemies->enemyList[i]);
        aEnemies->enemyList[i] = NULL;
    }
    free(aEnemies);
}

size_t NET_enemiesGetSize(Enemies aEnemies){
    return aEnemies->size;
}


int enemyDamaged(Enemies aEnemies, int damage, int index){
    if (!aEnemies || index < 0 || (size_t)index >= aEnemies->size || aEnemies->enemyList[index] == NULL){
        //fprintf(stderr,"enemyDamaged: invalid index %d (size = %zu)\n", index, aEnemies ? aEnemies->size : 0);
        return 0;
    }
    aEnemies->enemyList[index]->HP.currentHP -= damage;

    if (aEnemies->enemyList[index]->HP.currentHP <= 0) {
        NET_enemiesPopAt(aEnemies,(size_t)index);
        return 1;
    }
    return 0;
}

int enemyGetCount(Enemies aEnemies){
    return aEnemies->size;
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

float NET_enemiesCompute_dist(const Enemy e, SDL_Rect playerRect){
    float ex = e->hitbox.x + e->hitbox.w * 0.5f;
    float ey = e->hitbox.y + e->hitbox.h * 0.5f;

    float px = playerRect.x + playerRect.w * 0.5f;
    float py = playerRect.y + playerRect.h * 0.5f;
    
    float dx = fabsf(ex - px);
    float dy = fabsf(ey - py);
    return dx*dx + dy*dy;
}

int NET_enemiesCompEntries(const void *a, const void *b){
    const SortEntry *A = a, *B = b;
    if (A->dist < B->dist) return -1;
    if (A->dist > B->dist) return 1;
    return A->enemy->enemyID - B->enemy->enemyID;
}

SDL_Point NET_enemyGetPos(Enemy aEnemy){
    return (SDL_Point){.x = aEnemy->hitbox.x, .y = aEnemy->hitbox.y};
}

int NET_enemyGetDirection(Enemy aEnemy){
    return aEnemy->direction;
}
// int getEnemyHP(Enemies aEnemies, int index){
//     return aEnemies->enemyList[index]->HP.currentHP;
// }

int getEnemyHP(Enemy aEnemy){
    return aEnemy->HP.currentHP;
}