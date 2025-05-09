#include "../../include/NET/server.h"

struct Scenario{
    SDL_Point startPoint;
    SDL_Point objectivePoint;
    float spawnFrequency;
    int difficulty;
    ScenarioState scenario;
    int totalKilldEnemise;
    int waveCount;
    bool victory;
};

struct Weapon{
    int damage;
    int projFreq;
    int projSpeed;
    int projType;
};

struct Player{
    SDL_Rect hitBox;
    SDL_Point mousePos;
    int8_t angle;
    int direction;
    int character;
    Weapon weapon;
    int projCounter;
    bool isShooting;
    int dashCooldown;
    int lastDashTime;
    int HP;
    int maxHP;
};

struct User{
    IPaddress IP;
    char* username;
    int LobbyID;
    GameState State;
    Player player;
    int colorIndex;
    bool isHubVisible;
};

struct server {
    UDPsocket serverSocket;
    SDLNet_SocketSet socketSet;
    int clientCount;
    UDPpacket *pReceivePacket;
    UDPpacket *pSendPacket;
    User *clients;
    Projectile projList[1024];
    int projCount;
    bool isOff;
    ServerMap aServerMap;
    Enemies aEnemies;
    Scenario scenario;
    bool usedColors[MAX_COLORS];
};

static void keyPressedListener(Server aServer, int playerIdx, ServerMap aServerMap);

void* projektil_threads(void *arg);
void* enemies_threads(void *arg);

int stop = 0;
mutex_t stop_mutex;
thread_t projThread; 
thread_t enemyThread;


int main(int argc, char **argv ){
    (void)argc; (void)argv;
    NET_serverInitSDL();
    Server aServer = {0};

    aServer = NET_serverCreate();
    aServer->aEnemies = NET_enemiesCreate();
    memset(aServer->usedColors, 0, sizeof(aServer->usedColors));
    bool isRunning;
    aServer->aServerMap = NET_serverMapCreate();
    // if Server has allocated memory then the server is running on "PORT"
    if(aServer == NULL){
        isRunning = false;
    }else{
        isRunning = true;
        printf("UDP server started on port %d\n", PORT);
    }
    
    mutex_init(&stop_mutex);
    thread_create(&projThread, projektil_threads,aServer);
    thread_create(&enemyThread,enemies_threads,aServer);

    while (isRunning){
        int numReady = SDLNet_CheckSockets(aServer->socketSet, 10); 
        if (numReady == -1) {
            fprintf(stderr, "SDLNet_CheckSockets error: %s\n", SDLNet_GetError());
            break;
        }
        if(numReady > 0 && SDLNet_SocketReady(aServer->serverSocket)){
            while (SDLNet_UDP_Recv(aServer->serverSocket, aServer->pReceivePacket)){
                Packet aPacket = NET_packetDeserialize(aServer->pReceivePacket->data, aServer->pReceivePacket->len);
                if(aPacket == NULL){
                    printf("Deserialization failed! Buffer might be invalid.\n");
                    break;
                } 
                switch (NET_packetGetMessageType(aPacket)){
                case CONNECT:
                    NET_serverClientConnected(aPacket, aServer);
                    NET_serverSendPlayerPacket(aServer,-1);
                    break;
                case DISCONNECT:
                    NET_serverClientDisconnect(aServer);
                    // nedds to be addede when frendlist is a ting 
                    // updating the clients in the sesific lobby
                    break;
                case LOBBY_LIST:
                    printf("Hej det är %s\n", (char*)(NET_packetGetPayload(aPacket)));
                    break;
                case LOBBY_LIST_RESPONSE:// server kommer inte ta emot detta  
                    //PlayerPacket list[3] = {0};
                    //NET_PlayerListUpdate(aPacket,list,&n);
                    //NET_PlayerListPrintf(list,3);
                    break;
                case JOIN_LOBBY:
                    //lobbyID = (*(int*)NET_packetGetPayload(aPacket));
                    //aServer->clients[NET_serverCompIP(aServer)].LobbyID = lobbyID;
                    // send loby respone with list of players in lobby(brodcast till lobby)
                    break;
                case JOIN_LOBBY_RESPONSE:
                    break;
                case PRINT:
                    printf("%s\n",(char*)NET_packetGetPayload(aPacket));
                    NET_serverSendString(aServer,GLOBAL,PRINT,"hej hej\n",NET_serverCompIP(aServer));
                    break;
                case CHANGE_GAME_STATE:
                    NET_serverChangeGameStateOnClient(aServer, aPacket);
                    break;
                case PLAYER_INPUT:{
                    int index = NET_serverCompIP(aServer);
                    if(index == -1)break;;
                    NET_serverUpdatePlayer(aServer, aPacket,aServer->clients[index].State);
                    break;
                }
                case TRY_OPEN_TERMINAL_HUB_RESPONSE:{
                    int playerIdx = NET_serverCompIP(aServer); 
                    keyPressedListener(aServer, playerIdx, aServer->aServerMap);
                    break;
                }
                case PAUSE_MENU_REQUEST:{
                    int playerIdx = NET_serverCompIP(aServer); 
                    NET_serverSendInt(aServer, GLOBAL, PAUSE_MENU_CONFIRM, 1, playerIdx);
                    break;
                }
                default:
                    printf("Failed! msgType: %d\n", NET_packetGetMessageType(aPacket));
                    break;
                }
                if(aPacket) NET_packetDestroy(aPacket);
            }
            NET_serverScenarioCheckForVictory(aServer);
            if(aServer->isOff)break;
        }
    }
    mutex_lock(&stop_mutex);
    stop = 1;
    mutex_unlock(&stop_mutex);

    thread_join(projThread);
    thread_join(enemyThread);
    mutex_destroy(&stop_mutex);

    NET_enemiesDestroy(aServer->aEnemies);

    NET_serverDestroy(aServer);
    NET_serverDestroySDL();
    return 0;
}

void* enemies_threads(void *arg){
    Server aServer = (Server)arg;
    int previousTime = (int)SDL_GetTicks();
    // for (int i = 0; i < 20; i++){
    //     NET_enemiesPush(aServer->aEnemies,NET_enemyCreate(10+10*i,10+10*i,LIGHT_ENEMY));
    // }
    while (1){
        mutex_lock(&stop_mutex);
        int should_stop = stop;
        mutex_unlock(&stop_mutex);
        if(should_stop) break;
        NET_serverEnemiesSpawnInterval(aServer,&previousTime);
        NET_serverUpdateEnemies(aServer, aServer->aEnemies,aServer->aServerMap);
        sleep_ms(10);
    }
    printf("Enemise thread exiting. id: %lu\n",(unsigned long)thread_self());
    return NULL;
}


void* projektil_threads(void *arg){
    Server aServer = (Server)arg;
    while (1){
        mutex_lock(&stop_mutex);
        int should_stop = stop;
        mutex_unlock(&stop_mutex);
        if(should_stop) break;

        NET_projectilesUpdate(aServer, aServer->projList);
        NET_serverSendProjPacket(aServer);
        sleep_ms(10);
    }
    printf("Projectile thread exiting. id: %lu\n",(unsigned long)thread_self());
    return NULL;
}

void NET_serverEnemiesSpawnInterval(Server aServer,int *previousTime){
    if(aServer->scenario.victory) return;
    switch (aServer->scenario.scenario){
    case ELIMINATIONS:
        for (int i = 0; i < aServer->clientCount; i++){
        if(aServer->clients[i].State != MENU && 
        aServer->clients[i].State != LOBBY &&
        (int)SDL_GetTicks() >= 5000+(*previousTime) -(aServer->scenario.spawnFrequency*100) && 
        (int)NET_enemiesGetSize(aServer->aEnemies) < MAX_ENEMIES_CLIENT_SIDE)
        {
            //printf("enemy count %d\n",(int)NET_enemiesGetSize(aServer->aEnemies));
            (*previousTime) = SDL_GetTicks();
            NET_enemiesPush(aServer->aEnemies,NET_enemyCreate(50,50,LIGHT_ENEMY,aServer->scenario.difficulty));
        }
        }
        break;
    case WAVE:
        if((int)NET_enemiesGetSize(aServer->aEnemies) == 0){
            aServer->scenario.waveCount ++;
            for (int i = 0; i < aServer->clientCount; i++){
                if(aServer->clients[i].State == MENU || aServer->clients[i].State == LOBBY) continue;
                for (int i = 0; i < aServer->scenario.waveCount * 2; i++){
                    NET_enemiesPush(aServer->aEnemies,NET_enemyCreate(50,50,LIGHT_ENEMY,aServer->scenario.difficulty));
                }
            }
        }
        break;
    case PATH:
        for (int i = 0; i < aServer->clients; i++){
            if(aServer->clients[i].State != MENU && aServer->clients[i].State != LOBBY && 
            (int)SDL_GetTicks() >= 5000+(*previousTime) -(aServer->scenario.spawnFrequency*100) && 
            (int)NET_enemiesGetSize(aServer->aEnemies) < MAX_ENEMIES_CLIENT_SIDE)
            {
                (*previousTime) = SDL_GetTicks();
                NET_enemiesPush(aServer->aEnemies,NET_enemyCreate(50,50,LIGHT_ENEMY,aServer->scenario.difficulty));
            }
        }
        break;
    default:
        break;
    }
}

void NET_serverSendEnemiesPacket(Server aServer, GameState GS, Enemies aEnemies){
    EnemyPacket packet[MAX_ENEMIES_CLIENT_SIDE] = {0};
    SDL_Point pos;
    for (int i = 0; i <aServer->clientCount ; i++){
        User p = aServer->clients[i];
        if(p.State == MENU || p.State == LOBBY) continue;
        int enemySendCount = 0;
        size_t n = NET_enemiesGetSize(aServer->aEnemies);
        SortEntry *entries = malloc(n * sizeof *entries);
        for (size_t i = 0; i < n; ++i) {
            entries[i].enemy = NET_enemiesGetAt(aServer->aEnemies,i);
            entries[i].dist = NET_enemiesCompute_dist(entries[i].enemy, p.player.hitBox);
        }
        qsort(entries, n, sizeof *entries, NET_enemiesCompEntries);
        for (int i = 0; i < (int)NET_enemiesGetSize(aEnemies); i++){
            Enemy en = entries[i].enemy;
            pos = NET_enemyGetPos(en);
            if( abs(p.player.hitBox.y - pos.y) < MAX_ENEMIES_RANGE &&
                abs(p.player.hitBox.x - pos.x) < MAX_ENEMIES_RANGE){
                    if(enemySendCount < MAX_ENEMIES_CLIENT_SIDE - 1){
                        packet[enemySendCount].x = (int16_t)(p.player.hitBox.x - pos.x);
                        packet[enemySendCount].y = (int16_t)(p.player.hitBox.y - pos.y);
                        packet[enemySendCount].direction = (int16_t)(NET_enemyGetDirection(en));
                        packet[enemySendCount].hp = (uint8_t)getEnemyHP(en);
                        enemySendCount++;
                    }
            }
        }
        free(entries);
        Uint32 payloadSize = enemySendCount * sizeof(EnemyPacket);
        if((int)NET_enemiesGetSize(aEnemies) == 0) payloadSize = 1;
        UDPpacket* SendEnemies = SDLNet_AllocPacket(512);
        NET_protocolSendArray(SendEnemies,aServer->serverSocket, aServer->clients[i].IP, GLOBAL, ENEMY_POS, packet, payloadSize);
        //NET_serverSendArray(aServer, GLOBAL, ENEMY_POS, packet, payloadSize, i);
        SDLNet_FreePacket(SendEnemies);
    }
}

void NET_serverSetNewMap(Server aServer){
    int indexIP = NET_serverCompIP(aServer);
    if(indexIP == -1){
        printf("NET_serverCompIP == -1\n");
        return;
    }
    for (int i = 0; i < aServer->clientCount; i++){
        if(i == indexIP) continue;
        switch (aServer->clients[i].State){
        case NEMUR:case AURANTIC:case CINDORA:
            printf("VI HAR REDAN EN KARTA !!!\n");
            NET_serverSendInt(aServer,GLOBAL,NEW_SEED,(int)NET_serverMapGetSeed(aServer->aServerMap),indexIP);
            return;
            break;
        default:
            break;
        }
    }
    NET_serverMapSetSeed(aServer->aServerMap,MAP_generate_seed());
    NET_serverMapGenerateNewMap(aServer->aServerMap);
    NET_serverMapSetEdgesToZero(aServer->aServerMap);
    NET_serverSendInt(aServer,GLOBAL,NEW_SEED,(int)NET_serverMapGetSeed(aServer->aServerMap),indexIP);
    printf("%u\n",NET_serverMapGetSeed(aServer->aServerMap));
    aServer->scenario.scenario =1; //((int)NET_serverMapGetSeed(aServer->aServerMap) % SCENARIO_COUNT);
    printf("scnareo %d\n",aServer->scenario.scenario);
    NET_serverScenarioUpdate(&aServer->scenario,aServer->scenario.scenario,NET_serverMapGetSeed(aServer->aServerMap));
    NET_enemiesClear(aServer->aEnemies);
}

void NET_serverScenarioUpdate(Scenario *s, ScenarioState state, uint32_t seed){
    s->totalKilldEnemise = 0;
    s->victory = false;
    s->waveCount = 0;
    switch (state){
    case ELIMINATIONS:
        s->difficulty = 1;
        s->spawnFrequency = 9;
        s->startPoint = (SDL_Point){.x = 50, .y = 50};
        s->objectivePoint = (SDL_Point){.x = 0, .y = 0};
        break;
    case WAVE:
        s->difficulty = 1;
        s->spawnFrequency = 9;
        s->startPoint = (SDL_Point){.x = 50, .y = 50};
        s->objectivePoint = (SDL_Point){.x = 0, .y = 0};
        break;
    case PATH:
        s->difficulty = 1;
        s->spawnFrequency = 9;
        s->startPoint = (SDL_Point){.x = 50, .y = 50};
        s->objectivePoint = (SDL_Point){.x = seed % MAP_HEIGHT, .y = seed % MAP_WIDTH};
        break;
    default:
        printf("Not a Scenario\n");
        break;
    }
}

void NET_serverScenarioCheckForVictory(Server aServer){
    switch(aServer->scenario.scenario){
        case ELIMINATIONS:
            if(aServer->scenario.totalKilldEnemise == 50 * aServer->scenario.difficulty){
                aServer->scenario.victory = true;
            }
        break;
        case WAVE:
            if(aServer->scenario.totalKilldEnemise == 50 * aServer->scenario.difficulty){
                aServer->scenario.victory = true;
            }
        break;
        case PATH:
            for(int i = 0;i < aServer->clientCount; i++){
                if(aServer->clients[i].player.hitBox.x == aServer->scenario.objectivePoint.x &&
                    aServer->clients[i].player.hitBox.y == aServer->scenario.objectivePoint.y){
                    aServer->scenario.victory = true;
                }
            }
        break;
        default:
            printf("Not a Scenario\n");
        break;
    }
    //printf("kill count\n %d",aServer->scenario.totalKilldEnemise);
}

uint8_t NET_serverGetPercentage(int currentHP, int maxHP){
    if (maxHP <= 0) return 0; 
    float percent = ((float)currentHP / (float)maxHP) * 100.0f;
    return (uint8_t)(percent); 
}



void NET_serverSendPlayerPacket(Server aServer,GameState GS){
    PlayerPacket packet[MAX_CLIENTS] = {0};
    for (int i = 0; i < aServer->clientCount; i++){
        strncpy(packet[i].username, aServer->clients[i].username, MAX_USERNAME_LEN - 1);
        packet[i].username[MAX_USERNAME_LEN - 1] = '\0';

        packet[i].state = aServer->clients[i].State;
        SDL_Point pos = {
            .x = aServer->clients[i].player.hitBox.x + aServer->clients[i].player.hitBox.w/2,
            .y = aServer->clients[i].player.hitBox.y + aServer->clients[i].player.hitBox.h
        };
        packet[i].pos = pos;
        packet[i].direction = aServer->clients[i].player.direction;
        packet[i].colorIndex = aServer->clients[i].colorIndex;
        packet[i].playerCharacter = aServer->clients[i].player.character;
        packet[i].isShooting = aServer->clients[i].player.isShooting;
        packet[i].dashCoolDown = aServer->clients[i].player.dashCooldown;
        packet[i].HpProcent = NET_serverGetPercentage(aServer->clients[i].player.HP , aServer->clients[i].player.maxHP);
    }
    Uint32 payloadSize = aServer->clientCount * sizeof(PlayerPacket);
    for (int i = 0; i < aServer->clientCount; i++){
        if(aServer->clients[i].State == GS || GS == -1){
            NET_serverSendArray(aServer, GLOBAL, LOBBY_LIST, packet, payloadSize, i);
        }
    }
}

void NET_serverSendProjPacket(Server aServer) {
    for (int player = 0; player < aServer->clientCount; player++) {
        User p = aServer->clients[player];

        if (p.State == LOBBY || p.State == NEMUR) {
            ProjPacket packet[MAX_SERVER_PROJECTILES] = {0};
            int projSendCount = 0;

            for (int i = 0; i < aServer->projCount; i++) {
                if (abs(p.player.hitBox.x - aServer->projList[i].x) < CLIENT_PROJ_RANGE &&
                    abs(p.player.hitBox.y - aServer->projList[i].y) < CLIENT_PROJ_RANGE)
                {
                    if (projSendCount < MAX_CLIENT_PROJ) {
                        packet[projSendCount].angle = aServer->projList[i].angle;
                        packet[projSendCount].textureIdx = aServer->clients[aServer->projList[i].srcPlayerIdx].player.weapon.projType;
                        packet[projSendCount].x = p.player.hitBox.x - aServer->projList[i].x;
                        packet[projSendCount].y = p.player.hitBox.y - aServer->projList[i].y;
                        projSendCount++;
                    } else break;
                }
            } 

            for (int j = projSendCount; j < MAX_CLIENT_PROJ; j++) {
                packet[j].angle = 0;
                packet[j].x = 0;
                packet[j].y = 0;
                packet[j].textureIdx = PROJ_TEX_NONE;
            }

            Uint32 payloadSize = MAX_CLIENT_PROJ * sizeof(ProjPacket);
            UDPpacket* SendProj = SDLNet_AllocPacket(512);
            NET_protocolSendArray(SendProj,aServer->serverSocket, aServer->clients[player].IP,GLOBAL,PROJ_LIST,packet,payloadSize);
            //NET_serverSendArray(aServer, GLOBAL, PROJ_LIST, packet, payloadSize, player);
            SDLNet_FreePacket(SendProj);
        }
    }
}

static void calcMovement(Server aServer, PlayerInputPacket *pip, int playerIdx){

    float speed = 3.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    float dDash = 4.0f;

    // Directional input
    if (pip->keys[PLAYER_INPUT_UP])    dy -= 2.0f;
    if (pip->keys[PLAYER_INPUT_DOWN])  dy += 2.0f;
    if (pip->keys[PLAYER_INPUT_LEFT])  dx -= 2.0f;
    if (pip->keys[PLAYER_INPUT_RIGHT]) dx += 2.0f;

    // dash
    int time = SDL_GetTicks();
    aServer->clients[playerIdx].player.dashCooldown = (time - aServer->clients[playerIdx].player.lastDashTime) / 40;
    if (pip->keys[PLAYER_INPUT_SPACE] && (aServer->clients[playerIdx].player.dashCooldown < 10 || aServer->clients[playerIdx].player.dashCooldown > 100)){
        speed *= dDash;
        if(aServer->clients[playerIdx].player.dashCooldown > 100) {
            aServer->clients[playerIdx].player.lastDashTime = SDL_GetTicks();
            aServer->clients[playerIdx].player.dashCooldown = 0;
        }
    }
    if(aServer->clients[playerIdx].player.dashCooldown > 100) {
        aServer->clients[playerIdx].player.dashCooldown = 100;
    }

    // Normalize movement vector if diagonal
    if(dy && dx){
        dy = dy / 2;
    }

    if(MAP_TileNotWalkable(aServer->aServerMap,
        aServer->clients[playerIdx].player.hitBox.x + (int)dx * speed,
        aServer->clients[playerIdx].player.hitBox.y + (int)dy * speed,
        aServer->clients[playerIdx].State)) return;

    int collisionType;
    NET_serverCheckPlayerCollision(aServer, playerIdx, &collisionType);
    if(collisionType != 0) {
        speed = 1.0f;
    }

    aServer->clients[playerIdx].player.hitBox.x += (int)dx * speed;
    aServer->clients[playerIdx].player.hitBox.y += (int)dy * speed;
}


static void keyPressedListener(Server aServer, int playerIdx, ServerMap aServerMap){

    int tileX, tileY;
    MAP_ScreenToTile(aServerMap, aServer->clients[playerIdx].player.hitBox.x, aServer->clients[playerIdx].player.hitBox.y, &tileX, &tileY);
    

    bool inTerminalArea = 
        tileX >= 12 &&
        tileX <= 19 &&
        tileY >= 0 &&
        tileY <= 4;
            
    if (inTerminalArea) {

        int indexIP = NET_serverCompIP(aServer);
        if(indexIP == -1) {
            printf("Error NET_serverCompIP return -1\n");
            return;
        }
        aServer->clients[playerIdx].isHubVisible = !aServer->clients[playerIdx].isHubVisible;
        NET_serverSendInt(aServer, GLOBAL, TRY_OPEN_TERMINAL_HUB, aServer->clients[playerIdx].isHubVisible, indexIP);
    }
}

int NET_serverGetProjectileSpeed(Server aServer, int playerIdx) {
    return aServer->clients[playerIdx].player.weapon.projSpeed;
}

void NET_serverUpdatePlayer(Server aServer, Packet aPacket, GameState state){
    PlayerInputPacket pip;
    Uint8* payload = NET_packetGetPayload(aPacket);
    memcpy(&pip, payload, sizeof(PlayerInputPacket));
    int playerIdx = NET_serverCompIP(aServer); 
    calcMovement(aServer, &pip, playerIdx);

    int mx = aServer->clients[playerIdx].player.mousePos.x = pip.mousePos.x;
    int my = aServer->clients[playerIdx].player.mousePos.y = pip.mousePos.y;
    int dx = 0 - mx;
    int dy = 0 - my;

    float angle = atan2(dy, dx);
    aServer->clients[playerIdx].player.direction = ((int)roundf(angle / (float)M_PI_4) + 7 ) % 8;
    int character = aServer->clients[playerIdx].player.character = pip.selecterPlayerCharacter;
    switch(character) {
        case CHARACTER_BLUEFACE:
        aServer->clients[playerIdx].player.weapon.damage = 4;
        aServer->clients[playerIdx].player.weapon.projFreq = 30;
        aServer->clients[playerIdx].player.weapon.projSpeed = 14;
        aServer->clients[playerIdx].player.weapon.projType = PROJ_TEX_NEON_LASER;
        break;

        case CHARACTER_BIGGIE:
        aServer->clients[playerIdx].player.weapon.damage = 1;
        aServer->clients[playerIdx].player.weapon.projFreq = 2;
        aServer->clients[playerIdx].player.weapon.projSpeed = 20;
        aServer->clients[playerIdx].player.weapon.projType = PROJ_TEX_BULLET;
        break;

        case CHARACTER_CLEOPATRA:
        aServer->clients[playerIdx].player.weapon.damage = 10;
        aServer->clients[playerIdx].player.weapon.projFreq = 40;
        aServer->clients[playerIdx].player.weapon.projSpeed = 5;
        aServer->clients[playerIdx].player.weapon.projType = PROJ_TEX_PURPLE_LASER;
        break;
    }

    if (angle < 0.0f) {
        angle += 2.0f * M_PI;
    }
    aServer->clients[playerIdx].player.angle = (uint8_t)roundf(angle / (2.0f * M_PI) * 255.0f);
    int freq = aServer->clients[playerIdx].player.weapon.projFreq;
    aServer->clients[playerIdx].player.isShooting = pip.keys[PLAYER_INPUT_MOUSEDOWN] && !pip.keys[PLAYER_INPUT_SPACE];
    if(pip.keys[PLAYER_INPUT_MOUSEDOWN] && !pip.keys[PLAYER_INPUT_SPACE] && 
        (aServer->clients[playerIdx].player.projCounter)++%freq == 0) // TODO: change frequency depending on weapon/character
    {
        NET_projectileSpawn(aServer, aServer->projList, playerIdx);
    }

    NET_serverSendPlayerPacket(aServer,state); 
}

bool enemyAttackPlayer(Server aServer, int index, SDL_Rect enemyHitbox){
    if(!SDL_HasIntersection(&aServer->clients[index].player.hitBox, &enemyHitbox)){
        return false;
    }
    aServer->clients[index].player.HP -= 10;
    if(aServer->clients[index].player.HP <= 0){
        aServer->clients[index].player.HP = 0;
        NET_serverForceGameStateChange(aServer,LOBBY,index);
        aServer->clients[index].State = LOBBY;
    }
    // printf("Current HP: %d\n", aServer->clients[index].player.HP);
    return true;
}

void NET_serverForceGameStateChange(Server aServer, GameState state, int index){
    switch (state){
    case MENU:
        break;
    case LOBBY:
        aServer->clients[index].player.HP = aServer->clients[index].player.maxHP;
        aServer->clients[index].player.hitBox.x = 200;
        aServer->clients[index].player.hitBox.y = 800;
        aServer->clients[index].isHubVisible = !aServer->clients[index].isHubVisible;
        NET_serverSendInt(aServer,GLOBAL,CHANGE_GAME_STATE_RESPONSE,state,index);
        NET_serverSendInt(aServer,GLOBAL,TRY_OPEN_TERMINAL_HUB,aServer->clients[index].isHubVisible,index);
        break;
    case NEMUR:
        break;
    case CINDORA:
        break;
    case AURANTIC:
        break;
    default:
        printf("do not support thet State\n");
        break;
    }
}

void NET_serverUpdateEnemies(Server aServer, Enemies aEnemies, ServerMap aMap){
    int enemyCount = (int)NET_enemiesGetSize(aEnemies);
    if (enemyCount >= 0){
        for (int i = 0; i < enemyCount; i++) {
            float closestDist = INT_MAX;
            int closestPlayerIndex = -1;

            SDL_Point enemyPos = enemyGetPoint(aEnemies, i);

            for (int j = 0; j < aServer->clientCount; j++) {
                if(aServer->clients[j].State == LOBBY || aServer->clients[j].State == MENU) continue;
                SDL_Rect playerHitbox = aServer->clients[j].player.hitBox;
                int dx = playerHitbox.x - enemyPos.x;
                int dy = playerHitbox.y - enemyPos.y;
                int distSq = dx * dx + dy * dy;

                if (distSq < closestDist) {
                    closestDist = distSq;
                    closestPlayerIndex = j;
                }
            }
            SDL_Rect enemyHitbox = enemyGetHitbox(aEnemies, i);
            if (closestPlayerIndex != -1) {
                PlayerTracker(aEnemies, aServer, closestPlayerIndex, i);

                SDL_Point closestPos = {
                    aServer->clients[closestPlayerIndex].player.hitBox.x,
                    aServer->clients[closestPlayerIndex].player.hitBox.y
                };
                Uint32 currentTime = SDL_GetTicks(); 
                if(currentTime > enemyGetAttackTime(aEnemies, i) + 1000){
                    if(enemyAttackPlayer(aServer, closestPlayerIndex, enemyHitbox)){
                        NET_serverSendPlayerPacket(aServer, aServer->clients[closestPlayerIndex].State);
                        
                    }
                    enemySetAttackTime(aEnemies, i);
                }
                enemyAngleTracker(aEnemies, closestPos, i);
            }

            for (int j = 0; j < aServer->projCount; j++) {
                SDL_Rect projectileRect = {
                    .x = aServer->projList[j].x - PROJECTILEWIDTH / 2,
                    .y = aServer->projList[j].y - PROJECTILEHEIGHT / 2,
                    .w = PROJECTILEWIDTH,
                    .h = PROJECTILEWIDTH
                };
                if (enemyColitino(projectileRect, enemyHitbox)){
                    aServer->scenario.totalKilldEnemise += enemyDamaged(aEnemies, aServer->clients[aServer->projList[j].srcPlayerIdx].player.weapon.damage, i, &enemyCount);
                    NET_projectileKill(aServer, &aServer->projList[j], j);
                }
            }
        }
        NET_serverSendEnemiesPacket(aServer, NEMUR, aEnemies);
        // if (enemyCount == 0){
        //     enemyCount--;
        // }
    }
}

void NET_serverChangeGameStateOnClient(Server aServer,Packet aPacket){
    int indexIP = NET_serverCompIP(aServer);
    if(indexIP == -1) {
        printf("Error NET_serverCompIP return -1\n");
        return;
        }
    GameState newState = SDLNet_Read32(NET_packetGetPayload(aPacket));
    NET_serverSendInt(aServer,GLOBAL,CHANGE_GAME_STATE_RESPONSE,newState,indexIP);
    printf("username: %s gameState is now %d\n",aServer->clients[indexIP].username,newState);
    aServer->clients[indexIP].State = newState;
    if(newState == LOBBY){
        aServer->clients[indexIP].player.hitBox.x = 200;
        aServer->clients[indexIP].player.hitBox.y = 800;
        if(aServer->clients[indexIP].isHubVisible) {
            aServer->clients[indexIP].isHubVisible = !aServer->clients[indexIP].isHubVisible;
            NET_serverSendInt(aServer, GLOBAL, TRY_OPEN_TERMINAL_HUB, aServer->clients[indexIP].isHubVisible, indexIP);
        }
    } 
    else if(newState == MENU) {}
    else NET_serverSetNewMap(aServer);
}

void NET_serverClientDisconnect(Server aServer){
    int indexIP = NET_serverCompIP(aServer);
    int color = aServer->clients[indexIP].colorIndex;
    if (color >= 0 && color < MAX_COLORS)
    {
        aServer->usedColors[color] = false;
    }
    
    if(indexIP == -1) {
        printf("Error NET_serverCompIP return -1\n");
        return;
        }
    int lobbyID = aServer->clients[indexIP].LobbyID;
    for (int i = 0; i < aServer->clientCount; i++){
        if(aServer->clients[i].LobbyID == lobbyID){
            NET_serverSendPlayerPacket(aServer,-1);
        } 
    }
    printf("username: %s disconnected to server\n",aServer->clients[indexIP].username);
    NET_serverRemoveUser(aServer, indexIP);
    if(aServer->clientCount == 0) aServer->isOff = true;
}

int NET_serverCompIP(Server aServer){
    for (int i = 0; i < aServer->clientCount; i++){
        if (aServer->clients[i].IP.host == aServer->pReceivePacket->address.host && 
            aServer->clients[i].IP.port == aServer->pReceivePacket->address.port ){
            return i;
        }
    }
    return -1;
}

SDL_Rect NET_serverGetPlayerHitbox(Server aServer, int playerIndex) {
    return aServer->clients[playerIndex].player.hitBox;
}

void NET_serverSetPlayerHitbox(Server aServer, int playerIndex, SDL_Rect r) {
    aServer->clients[playerIndex].player.hitBox = r;
}

int NET_serverGetClientCount(Server aServer) {
    return aServer->clientCount;
}

void NET_serverDestroy(Server aServer){
    if(aServer->pReceivePacket != NULL){
        SDLNet_FreePacket(aServer->pReceivePacket);
        aServer->pReceivePacket = NULL;
    }
    if(aServer->pSendPacket != NULL){
        SDLNet_FreePacket(aServer->pSendPacket); 
        aServer->pSendPacket = NULL;
    }
    if(aServer->socketSet != NULL){
        SDLNet_FreeSocketSet(aServer->socketSet);
        aServer->socketSet = NULL;
    }
    if(aServer->serverSocket != NULL) {
        SDLNet_UDP_Close(aServer->serverSocket);
        aServer->serverSocket = NULL;
    }
    free(aServer->clients);
    NET_serverMapDestroy(aServer->aServerMap);
    free(aServer);
}

int NET_serverGetProjCount(Server aServer) {
    return aServer->projCount;
}

void NET_serverSetProjCount(Server aServer, int count) {
    aServer->projCount = count;
}

float NET_serverGetPlayerAngle(Server aServer, int playerIdx) {
    return aServer->clients[playerIdx].player.angle;
}

Server NET_serverCreate(){
    Server aServer = malloc(sizeof(struct server));
    if(aServer == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }
    aServer->clients = NULL;
    aServer->clientCount = 0;
    aServer->isOff = false;
    aServer->projCount = 0;
     // Open server UDP socket
    aServer->serverSocket = SDLNet_UDP_Open(PORT);
    if(!aServer->serverSocket){
        printf("Failed to open UDP socket on port %d\n",PORT);
        return NULL;
    }
    aServer->socketSet = SDLNet_AllocSocketSet(1);
    if (!aServer->socketSet) {
        fprintf(stderr, "SDLNet_AllocSocketSet failed: %s\n", SDLNet_GetError());
        return NULL;
    }
    if (SDLNet_UDP_AddSocket(aServer->socketSet, aServer->serverSocket) == -1) {
        fprintf(stderr, "SDLNet_UDP_AddSocket failed: %s\n", SDLNet_GetError());
        return NULL;
    }
    // Packets for sending/receiving
    aServer->pSendPacket = SDLNet_AllocPacket(512);
    aServer->pReceivePacket = SDLNet_AllocPacket(512);
    if (!aServer->pReceivePacket || !aServer->pSendPacket) {
        printf("Failed to allocate packets\n");
        return NULL;
    }
    return aServer;
} 

// Functions to send datatypes to the clients
void NET_serverSendInt(Server aServer,GameState GS, MessageType msgType,int placeHolder, int index){
    NET_protocolSendInt(aServer->pSendPacket, aServer->serverSocket, aServer->clients[index].IP, GS, msgType, placeHolder);
}

void NET_serverSendString(Server aServer,GameState GS, MessageType msgType, const char* str, int index){
    NET_protocolSendString(aServer->pSendPacket, aServer->serverSocket, aServer->clients[index].IP, GS, msgType, str);
}

void NET_serverSendArray(Server aServer,GameState GS, MessageType msgType, const void* array, Uint32 arraySize, int index){
    NET_protocolSendArray(aServer->pSendPacket, aServer->serverSocket, aServer->clients[index].IP, GS, msgType, array, arraySize);
}

void NET_serverRemoveUser(Server aServer,int index){
    if(index < 0 || index >= aServer->clientCount){
        printf("Invalid index\n");
        return;
    }
    free(aServer->clients[index].username);
    for (int i = index; i < aServer->clientCount - 1; i++){
        aServer->clients[i] = aServer->clients[i + 1];
    }
    // User* temp = NULL;
    // if(aServer->clientCount - 1 > 0){
    //     temp = realloc(aServer->clients, (aServer->clientCount - 1) * sizeof(User));
    //     if(temp == NULL){
    //         printf("Realloc failed when removing user\n");
    //         return;
    //     }
    // }
    // aServer->clients = temp;
    aServer->clientCount--;
}

void NET_serverAddUser(Server aServer, User newUser){
    User* temp = realloc(aServer->clients, (aServer->clientCount + 1) * sizeof(User));
    if(temp != NULL){
        aServer->clients = temp;
        aServer->clients[aServer->clientCount] = newUser;
        aServer->clientCount++;
    }
    else{
        printf("Realloc failed when adding a user!\n");
    }
}

void NET_serverClientConnected(Packet aPacket, Server aServer){
    User newUser = {0};
    char *usernameFromPacket = (char*)NET_packetGetPayload(aPacket);
    newUser.username = strdup(usernameFromPacket);
    if(newUser.username == NULL){
        fprintf(stderr, "Failed to allocate memory for username\n");
        return;
    }
    newUser.IP = aServer->pReceivePacket->address;
    newUser.LobbyID = -1;
    // newUser.State = NET_packetGetMessageType(aPacket);
    newUser.State = MENU;
    newUser.player.hitBox.x = 200;
    newUser.player.hitBox.y = 800;
    newUser.player.hitBox.w = 64;
    newUser.player.hitBox.h = 32;
    newUser.player.projCounter = 0;
    newUser.player.weapon.damage = 1;
    newUser.player.weapon.projFreq = 10;
    newUser.player.weapon.projSpeed = 14;
    // Maybe init HP when switching to a character
    newUser.player.HP = 100;
    newUser.player.maxHP = 100;
    newUser.player.weapon.projType = PROJ_TEX_BULLET;
    newUser.player.dashCooldown = 100;
    newUser.State = MENU;
    newUser.colorIndex = NET_serverAssignColorIndex(aServer);
    newUser.isHubVisible = false;
    NET_serverAddUser(aServer, newUser);
    NET_serverSendInt(aServer, GLOBAL, CONNECT_RESPONSE, 0, aServer->clientCount - 1);
    printf("username: %s connected to server\n", aServer->clients[aServer->clientCount - 1].username);
}

int NET_serverFindPlayerID(Server aServer, const char* str){
    for (int i = 0; i < aServer->clientCount; i++){
        if(strcmp(str, aServer->clients[i].username) == 0){
            return (i);
        }
    }
    return -1;
}

int NET_serverAssignColorIndex(Server aServer){
    for (int i = 0; i < MAX_COLORS; i++)
    {
        if (!aServer->usedColors[i])
        {
            aServer->usedColors[i] = true;
            return i;
        }
    }
    return -1;
}

