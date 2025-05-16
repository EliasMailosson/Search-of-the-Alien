#include "../../include/NET/client.h"
#include "../../include/UI/friend.h"
#include "../../include/menu.h"
#include <stdio.h>

struct scenC{
    ScenarioState state;
    SDL_Point objectivePoint; 
    int Wave;
    int killCount;
};
struct WeaponStats {
    int type;
    int level;
};

typedef struct gameConfig {
    int nextGraphicsQuality;
    int graphicsQuality; // 1: high, 2: medium
}GameConfig;

struct client{
    SDLNet_SocketSet socketSet;
    UDPsocket clientSocket;
    UDPpacket *pReceivePacket;
    UDPpacket *pSendPacket;
    IPaddress serverAddr;
    char selfUsername[MAX_USERNAME_LEN]; //andras usernames // va?
    GameConfig config;
    int PlayerCount;
    Player playerList[MAX_CLIENTS];
    int EnemiesCount;
    ClientEnemy enemies[MAX_ENEMIES_CLIENT_SIDE];
    bool isHubVisible;
    bool showPauseMenu;
    uint32_t seed;
    Proj projList[MAX_CLIENT_PROJ];
    WeaponStats weaponStatList[3];
    ScenC scenario; 
}; 

bool NET_clientConnect(Client aClient){
    char* serverIP = NET_clientReadFileOne(SERVER_IP_PAHT);
    if (SDLNet_ResolveHost(&aClient->serverAddr,serverIP, PORT) < 0) {
        printf("SDLNet_ResolveHost failed: %s\n", SDLNet_GetError());
        free(serverIP);
        return false;
    }
    free(serverIP);
    return true;
}

int NET_clientGetGraphicsQuality(Client aClient) {
    return aClient->config.graphicsQuality;
}

void NET_clientGetProjList(Client aClient, Proj *outputProjList) {
    for(int i = 0; i < MAX_CLIENT_PROJ; i++) {
        outputProjList[i].angle = aClient->projList[i].angle;
        outputProjList[i].textureIdx = aClient->projList[i].textureIdx;
        outputProjList[i].x = aClient->projList[i].x;
        outputProjList[i].y = aClient->projList[i].y;
    }
}

int NET_clientGetProjTexture(Client aClient, int projIdx) {
    return aClient->projList[projIdx].textureIdx;
}

ClientEnemy *NET_clientGetEnemy(Client aClient, int idx) {
    return &aClient->enemies[idx];
}

Player *NET_clientGetPlayer(Client aClient, int idx) {
    return &aClient->playerList[idx];
}

bool NET_clientIsMyUsername(Client aClient, char *name) {
    if(strcmp(name, aClient->selfUsername) == 0) {
        return true;
    } else return false;
}

Client NET_clientCreate(){

    Client aClient = malloc(sizeof(struct client));
    // Open a local UDP port (0 picks any available)
    aClient->clientSocket = SDLNet_UDP_Open(0);
    if (!aClient->clientSocket) {
        printf("SDLNet_UDP_Open failed: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(aClient->clientSocket);
        return NULL;
    }
    aClient->socketSet = SDLNet_AllocSocketSet(1);
    if (!aClient->socketSet) {
        fprintf(stderr, "SDLNet_AllocSocketSet failed: %s\n", SDLNet_GetError());
        return NULL;
    }
        if (SDLNet_UDP_AddSocket(aClient->socketSet, aClient->clientSocket) == -1) {
        fprintf(stderr, "SDLNet_UDP_AddSocket failed: %s\n", SDLNet_GetError());
        return NULL;
    }
       // Allocate packets
    aClient->pReceivePacket = SDLNet_AllocPacket(512);
    aClient->pSendPacket = SDLNet_AllocPacket(512);
    if (! aClient->pSendPacket || !aClient->pReceivePacket) {
        printf("Failed to allocate packets.\n");
        if (aClient->pSendPacket) SDLNet_FreePacket(aClient->pSendPacket);
        if (aClient->pReceivePacket) SDLNet_FreePacket(aClient->pReceivePacket);
        SDLNet_UDP_Close(aClient->clientSocket);
        return NULL;
    }

    NET_clientReadGraphicsConfig(aClient);
    aClient->config.nextGraphicsQuality = aClient->config.graphicsQuality;

    aClient->PlayerCount = 1;
    aClient->playerList[0].state = MENU;
    strcpy(aClient->playerList[0].username,"None");
    strcpy(aClient->selfUsername,"None");
    aClient->isHubVisible = false;
    aClient->seed = 0;
    aClient->EnemiesCount = 0;
    aClient->showPauseMenu = 0;
    NET_clientLoadWeaponStats(aClient);

    //OBJprogress default
    aClient->scenario.killCount = 0;
    aClient->scenario.Wave = 0;
    aClient->scenario.state = 0;
    
    return aClient;
}
void NET_clientGetPlayerName(Client aClient, int playerIndex, char* username) {
    if (playerIndex >= 0 && playerIndex < aClient->PlayerCount) {
        strncpy(username, aClient->playerList[playerIndex].username, MAX_USERNAME_LEN);
        username[MAX_USERNAME_LEN - 1] = '\0'; 
    } else {
        strcpy(username, "Starta om spelet om du vill ha ett namn bror");
    }
}

void NET_clientReadGraphicsConfig(Client aClient) {
    FILE *file = fopen("data/graphics.config", "r");
    if (!file) {
        aClient->config.graphicsQuality = 2;
        return;
    }

    char buf[256];
    while (fgets(buf, sizeof(buf), file)) {
        sscanf(buf, "QUALITY=%d", &aClient->config.graphicsQuality);
    }
    fclose(file);
}

void NET_clientSetNextGraphicsConfig(Client aClient, int value) {
    aClient->config.nextGraphicsQuality = value;
}

int NET_clientGetNextGraphicsConfig(Client aClient) {
    return aClient->config.nextGraphicsQuality;
}

void NET_clientSaveGraphicsConfig(Client aClient) {
    FILE *file = fopen("data/graphics.config", "w");
    fprintf(file, "QUALITY=%d", aClient->config.nextGraphicsQuality);
    fclose(file);
}

void NET_clientSetPlayerAnimation(Client aClient, int playerIdx, int newAnimation) {
    aClient->playerList[playerIdx].currentPlayerAnimation = newAnimation;
}

int NET_clientGetPlayerAnimation(Client aClient, int playerIdx) {
    return aClient->playerList[playerIdx].currentPlayerAnimation;
}

void NET_clientSetSelfName(Client aClient, char* newName) {
    strcpy(aClient->selfUsername,newName);
    strcpy(aClient->playerList[0].username, newName);
}

void NET_clientGetSelfname(Client aClient, char* outputName){
    strcpy(outputName, aClient->selfUsername);
}

int NET_clientGetSelfIndex(Client aClient){
    return NET_clientFindPlayer(aClient, aClient->selfUsername);
}

void NET_clientGetFriendsName(Client aClient, char outputNames[][MAX_USERNAME_LEN]) {
    for (int i = 0; i < aClient->PlayerCount; i++) {
        strcpy(outputNames[i],aClient->playerList[i].username);
    }
}

int NET_clientGetState(Client aClient) {
    for (int i = 0; i < aClient->PlayerCount; i++){
        if(strcmp(aClient->selfUsername,aClient->playerList[i].username) == 0) return aClient->playerList[i].state;
    }
    return -1;
}

 bool NET_clientIsPlayerDamaged(Client aClient, int selfIndex){
    static uint8_t lastHealth[8] = {100, 100, 100, 100, 100, 100, 100, 100};
    if(aClient->playerList[selfIndex].HpProcent < lastHealth[selfIndex]){
        lastHealth[selfIndex] = aClient->playerList[selfIndex].HpProcent;
        int hp = (int)aClient->playerList[selfIndex].HpProcent;
        //printf("Your HP = %d\n", hp);
        return true;
    }else{
        if(aClient->playerList[selfIndex].HpProcent == 100){
            lastHealth[selfIndex] = 100;
        }
    }
    return false;
}

int NET_clientGetPlayerCount(Client aClient) {
    return aClient->PlayerCount;
}
const Player *NET_clientGetPlayerList(Client aClient) {

    return aClient->playerList;
}

SDL_Point NET_clientGetPlayerPos(Client aClient, int playerIdx) {
    if(playerIdx < MAX_CLIENTS) {
        return aClient->playerList[playerIdx].pos;
    }
    else return (SDL_Point) {.x=-1, .y=-1};
}

SDL_Point NET_clientGetEnemyPos(Client aClient, int index){
    if(index < aClient->EnemiesCount) {
        return aClient->enemies[index].pos;
    }
    else return (SDL_Point) {.x=-1, .y=-1};
}

void NET_clientDestroy(Client aClient){
    NET_clientSaveWeaponStats(aClient);
    NET_clientSaveGraphicsConfig(aClient);

    if(aClient->pReceivePacket != NULL){
        SDLNet_FreePacket(aClient->pReceivePacket);
        aClient->pReceivePacket = NULL;
    }
    if(aClient->pSendPacket != NULL){
        SDLNet_FreePacket(aClient->pSendPacket); 
        aClient->pSendPacket = NULL;
    }
    if(aClient->socketSet != NULL){
        SDLNet_FreeSocketSet(aClient->socketSet);
        aClient->socketSet = NULL;
    }
    if(aClient->clientSocket != NULL) {
        SDLNet_UDP_Close(aClient->clientSocket);
        aClient->clientSocket = NULL;
    }
    free(aClient);
}

void NET_clientSendInt(Client aClient,GameState GS, MessageType msgType,int placeHolder){
    NET_protocolSendInt(aClient->pSendPacket,aClient->clientSocket,aClient->serverAddr,GS,msgType,placeHolder);
}

void NET_clientSendString(Client aClient,GameState GS, MessageType msgType,const char* str){
    NET_protocolSendString(aClient->pSendPacket, aClient->clientSocket, aClient->serverAddr, GS, msgType, str);
}

void NET_clientSendArray(Client aClient,GameState GS, MessageType msgType,const void* array, Uint32 arraySize){
    NET_protocolSendArray(aClient->pSendPacket, aClient->clientSocket, aClient->serverAddr, GS, msgType, array, arraySize);
}

void NET_clientReceiver(Client aClient, Map aMap,SDL_Window *pScreen){
    int numReady = SDLNet_CheckSockets(aClient->socketSet, 10); 
    if (numReady == -1) {
        fprintf(stderr, "SDLNet_CheckSockets error: %s\n", SDLNet_GetError());
        return;
    }
    if(numReady > 0 && SDLNet_SocketReady(aClient->clientSocket)){
        while (SDLNet_UDP_Recv(aClient->clientSocket, aClient->pReceivePacket)){
            Packet aPacket = NET_packetDeserialize(aClient->pReceivePacket->data, aClient->pReceivePacket->len);
            if(aPacket == NULL){
                printf("Deserialization for Client failed! Buffer might be invalid.\n");
                break;
            } 
            //Handle recieving packets from the server
            switch (NET_packetGetMessageType(aPacket)){
            case CONNECT_RESPONSE:
                // printf("received %d\n",*((int*)NET_packetGetPayload(aPacket)));
                break;
            case DISCONNECT_RESPONSE:
                // playerID = (char*)NET_packetGetPayload(aPacket);
                // index = NET_clientFindPlayer(aClient,playerID);
                // NET_PlayerListRemovePlayer(&aClient->playerList[NET_clientFindPlayer(aClient,playerID)],
                //                             index,&aClient->PlayerCount);
                NET_clientUpdatePlayerList(aClient,aPacket);
                break;
            case JOIN_LOBBY_RESPONSE:
                //NET_PlayerListUpdate(aPacket,aClient->list,&aClient->PlayerCount);
                break;
            case LOBBY_LIST:
                NET_clientUpdatePlayerList(aClient,aPacket);
                //printf("%d\n",(int)SDL_GetTicks());
                break;
            case PRINT:
                printf("%s\n",NET_packetGetPayload(aPacket));
                break;
            case CHANGE_GAME_STATE_RESPONSE:
                NET_clientUpdateGameState(aClient,aPacket);
                int w = 0,h = 0;
                SDL_GetWindowSize(pScreen,&w,&h);
                MAP_MapRefresh(aMap,w,h);
                break;
            case NEW_SEED:
                aClient->seed = SDLNet_Read32(NET_packetGetPayload(aPacket));
                printf("new seed: %u\n",aClient->seed);
                MAP_mapSetPlanet(NET_clientGetState(aClient),aMap);
                MAP_mapNewMap(aMap,aClient->seed);
                NET_clientScenarioUpdate(aClient,aClient->seed);
                aClient->scenario.killCount = 0;
                aClient->scenario.Wave = 0;
                break;
            case PROJ_LIST:
                NET_clientUpdateProjList(aClient, aPacket);
                break;
            case ENEMY_POS:
                NET_clientUpdateEnemy(aClient, aPacket);
            case TRY_OPEN_TERMINAL_HUB:
                aClient->isHubVisible = !aClient->isHubVisible;
                break;
            case PAUSE_MENU_CONFIRM:
                aClient->showPauseMenu = !aClient->showPauseMenu;
                break;
            case GET_WAVE:
                aClient->scenario.Wave = SDLNet_Read32(NET_packetGetPayload(aPacket));
                break;
            case KILLCOUNT:
                aClient->scenario.killCount = SDLNet_Read32(NET_packetGetPayload(aPacket));
                break;
            default:
                printf("client recieved invalid msgType: %d!!\n", NET_packetGetMessageType(aPacket));
                break;
            }
            if(aPacket) NET_packetDestroy(aPacket);
        }
    }
}


void NET_clientScenarioUpdate(Client aClient,uint32_t seed){
    aClient->scenario.state = abs(seed % SCENARIO_COUNT);
    switch (aClient->scenario.state){
    case ELIMINATIONS:
        aClient->scenario.objectivePoint = (SDL_Point){.x = 0, .y = 0};
        break;
    case WAVE:
        aClient->scenario.objectivePoint = (SDL_Point){.x = 0, .y = 0};
        break;
    case PATH:
        aClient->scenario.objectivePoint = (SDL_Point){.x = seed % MAP_HEIGHT, .y = seed % MAP_WIDTH};
        break;
    default:
        printf("NO SCENARIO\n");
        break;
    }
}

bool NET_clientGetPauseState(Client aClient) {
    return aClient->showPauseMenu;
}

int NET_clientGetClientState(Client aClient, int playerIdx) {
    return aClient->playerList[playerIdx].state;
}

int NET_clientGetPlayerDirection(Client aClient, int playerIdx) {
    return aClient->playerList[playerIdx].direction;
}

int NET_clientGetEnemyDirection(Client aClient, int index) {
    if (index < 0 || index >= MAX_ENEMIES_CLIENT_SIDE) {
        fprintf(stderr, "ERROR: enemy index %d out of bounds\n", index);
        return -1;
    }
    return aClient->enemies[index].direction;
}

int NET_clientGetPlayerCharacter(Client aClient, int playerIdx) {
    return aClient->playerList[playerIdx].playerCharacter;
}

void NET_clientUpdatePlayerList(Client aClient, Packet aPacket){
    PlayerPacket packets[MAX_CLIENTS] = {0};
    NET_playerPacketReceive(aPacket, packets, &aClient->PlayerCount);
    for (int i = 0; i < aClient->PlayerCount; i++){
        aClient->playerList[i].pos = packets[i].pos;
        aClient->playerList[i].state = packets[i].state;
        aClient->playerList[i].direction = packets[i].direction;
        aClient->playerList[i].colorIndex = packets[i].colorIndex;
        strcpy(aClient->playerList[i].username, packets[i].username);
        aClient->playerList[i].color = NET_clientGetColor(aClient->playerList[i].colorIndex);
        aClient->playerList[i].playerCharacter = packets[i].playerCharacter;
        aClient->playerList[i].isShooting = packets[i].isShooting;
        aClient->playerList[i].dashCooldown = packets[i].dashCoolDown;
        aClient->playerList[i].HpProcent = packets[i].HpProcent;
    }
}

int NET_clientGetDashCooldown(Client aClient) {
    return aClient->playerList[NET_clientGetSelfIndex(aClient)].dashCooldown;
}

void NET_clientUpdateEnemy(Client aClient, Packet aPacket){
    EnemyPacket packets[MAX_ENEMIES_CLIENT_SIDE] = {0};
    NET_enemyPacketReceive(aPacket, packets,&aClient->EnemiesCount);
    //printf("%d enemies for client\n",aClient->EnemiesCount);
    for (int i = 0; i < aClient->EnemiesCount; i++){
        aClient->enemies[i].pos.x = (int)packets[i].x;
        aClient->enemies[i].pos.y = (int)packets[i].y;
        aClient->enemies[i].direction = (int)packets[i].direction;
        aClient->enemies[i].hpEnemy = (uint8_t)packets[i].hp;
    }

    for (int i = aClient->EnemiesCount; i < MAX_ENEMIES_CLIENT_SIDE; i++) {
        aClient->enemies[i] = (ClientEnemy){0};
    }
}

int NET_clientGetEnemiesCount(Client aClinet){
    return aClinet->EnemiesCount;
}

int NET_clientIsShooting(Client aClient, int playerIdx) {
    return aClient->playerList[playerIdx].isShooting;
}

void NET_clientUpdateProjList(Client aClient, Packet aPacket) {
    ProjPacket packets[MAX_CLIENT_PROJ] = {0};
    int count;
    NET_projPacketReceive(aPacket, packets, &count);

    if(count <= MAX_CLIENT_PROJ) {
        for(int i = 0; i < MAX_CLIENT_PROJ; i++) {
            aClient->projList[i].angle = packets[i].angle;
            aClient->projList[i].textureIdx = packets[i].textureIdx;
            aClient->projList[i].x = packets[i].x;
            aClient->projList[i].y = packets[i].y;
        }
    }
}

void NET_clientUpdateGameState(Client aClient,Packet aPacket){
    GameState newState = SDLNet_Read32(NET_packetGetPayload(aPacket));
    aClient->playerList[NET_clientFindPlayer(aClient,aClient->selfUsername)].state = newState;
}

int NET_clientFindPlayer(Client aClient, char* str){
    for (int i = 0; i < aClient->PlayerCount; i++){
        if(strcmp(str, aClient->playerList[i].username) == 0){
            return i;
        }   
    }
    return -1;
}

SDL_Point NET_clientGetSelfPos(Client aClient){
    for (int i = 0; i < aClient->PlayerCount; i++){
        if(strcmp(aClient->selfUsername,aClient->playerList[i].username) == 0){
            return aClient->playerList[i].pos;
        }
    }
    return (SDL_Point){-1,-1};
}

int NET_clientGetHP(Client aClient, int index){
    return aClient->playerList[index].HpProcent;
}

SDL_Color NET_clientGetColor(int index){
    SDL_Color colors[] = {
        {255, 0, 0, 255},     // Red
        {0, 255, 0, 255},     // Green
        {0, 0, 255, 255},     // Blue
        {255, 255, 0, 255},   // Yellow
        {255, 0, 255, 255},   // Magenta
        {0, 255, 255, 255},   // Cyan
        {255, 165, 0, 255},   // Orange
        {128, 0, 128, 255},   // Purple
    };
    return colors[index];
}

SDL_Color NET_GetPlayerColor(Client aClient,int index){
    return aClient->playerList[index].color;
}

int NET_clientGetPlayerColorIndex(Client aClient,int index){
    return aClient->playerList[index].colorIndex;
}

bool NET_clientGetTerminalHub(Client aClient){
    return aClient->isHubVisible;
}

void NET_clientLoadWeaponStats(Client aClient) {
    FILE *fp ;
    fp = fopen("data/weapon-stats.csv", "r");
    if (fp != NULL)
    {   
        char buf[32];
        for(int i = 0; i < 3; i++) {
            fgets(buf, 32, fp);
            sscanf(buf, "%d,%d", &aClient->weaponStatList[i].type, &aClient->weaponStatList[i].level);
        }
    }

    fclose(fp);
}

void NET_clientSaveWeaponStats(Client aClient) {
    FILE *fp ;
    fp = fopen("data/weapon-stats.csv", "w");
    if (fp != NULL)
    {   
        for(int i = 0; i < 3; i++) {
            fprintf(fp, "%d,%d\n", aClient->weaponStatList[i].type, aClient->weaponStatList[i].level);
        }
    }

    fclose(fp);
}

int NET_clientGetEnemyHP(Client aClient, int index){
    return aClient->enemies[index].hpEnemy;
}

int NET_clientGetKillCount(Client aClient) {
    return aClient->scenario.killCount;
}

int NET_clientGetWaveCount(Client aClient) {
    return aClient->scenario.Wave;
}

ScenarioState NET_clientGetScenarioState(Client aClient) {
    return aClient->scenario.state;
}

SDL_Point NET_clientGetObjectivePoint(Client aClient) {
    return aClient->scenario.objectivePoint;
}

char* NET_clientReadFileOne(char* file){
    FILE *fp = fopen(file,"r");
    if(!fp){
        printf("Error openig: %s\n",file);
        return strdup("127.0.0.1");
    }
    char buf[MAX_LINE];
    char *out;
    if (fgets(buf, sizeof buf, fp)) {
        buf[strcspn(buf, "\n")] = '\0';
        out = strdup(buf);
        if (!out) {
            printf("strdup\n");
            out = strdup("127.0.0.1");
        }
    } else {
        out = strdup("127.0.0.1");
    }
    fclose(fp);
    return out;
}