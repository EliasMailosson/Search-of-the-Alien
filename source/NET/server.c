#include "../../include/NET/server.h"

struct Player{
    SDL_Rect hitBox;
    SDL_Point mousePos;
    int8_t angle;
    int direction;
    int character;
    int projCounter;
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
    bool usedColors[MAX_COLORS];
};

static void keyPressedListener(Server aServer, int playerIdx, ServerMap aServerMap);

int main(int argc, char **argv ){
    (void)argc; (void)argv;
    NET_serverInitSDL();
    Server aServer = {0};
    aServer = NET_serverCreate();
    memset(aServer->usedColors, 0, sizeof(aServer->usedColors));
    bool isRunning;
    int sendProjectileCounter = 0;
    aServer->aServerMap = NET_serverMapCreate();
    // if Server has allocated memory then the server is running on "PORT"
    if(aServer == NULL){
        isRunning = false;
    }else{
        isRunning = true;
        printf("UDP server started on port %d\n", PORT);
    } 
    while (isRunning){
        NET_projectilesUpdate(aServer, aServer->projList);
        if(sendProjectileCounter++%5 == 0) {
            NET_serverSendProjPacket(aServer);
        } 
        
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
                case PLAYER_INPUT:
                    int index = NET_serverCompIP(aServer);
                    if(index == -1)break;;
                    NET_serverUpdatePlayer(aServer, aPacket,aServer->clients[index].State);
                    break;
                case TRY_OPEN_TERMINAL_HUB_RESPONSE:{
                    int playerIdx = NET_serverCompIP(aServer); 
                    keyPressedListener(aServer, playerIdx, aServer->aServerMap);
                    break;
                }
                default:
                    printf("Failed!\n");
                    break;
                }
                if(aPacket) NET_packetDestroy(aPacket);
            }
            if(aServer->isOff)break;
        }
    }
    NET_serverDestroy(aServer);
    NET_serverDestroySDL();
    return 0;
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
                        packet[projSendCount].textureIdx = PROJ_TEX_BULLET;
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
            NET_serverSendArray(aServer, GLOBAL, PROJ_LIST, packet, payloadSize, player);
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
    if (pip->keys[PLAYER_INPUT_SPACE]){
        speed *= dDash;
    }

    // Normalize movement vector if diagonal
    if(dy && dx){
        dy = dy / 2;
    }

    if(MAP_TileNotWalkable(aServer->aServerMap, aServer->clients[playerIdx].player.hitBox.x + (int)dx * speed, aServer->clients[playerIdx].player.hitBox.y + (int)dy * speed)) return;

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
    aServer->clients[playerIdx].player.character = pip.selecterPlayerCharacter;
    if (angle < 0.0f) {
        angle += 2.0f * M_PI;
    }
    aServer->clients[playerIdx].player.angle = (uint8_t)roundf(angle / (2.0f * M_PI) * 255.0f);

    if(pip.keys[PLAYER_INPUT_MOUSEDOWN] && !pip.keys[PLAYER_INPUT_SPACE] && 
        (aServer->clients[playerIdx].player.projCounter)++%4 == 0) // TODO: change frequency depending on weapon/character
    {
        NET_projectileSpawn(aServer, aServer->projList, playerIdx);
    }

    NET_serverSendPlayerPacket(aServer,state); 
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
    if(newState == LOBBY || newState == MENU){}else NET_serverSetNewMap(aServer);
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
    User* temp = NULL;
    if(aServer->clientCount - 1 > 0){
        temp = realloc(aServer->clients, (aServer->clientCount - 1) * sizeof(User));
        if(temp == NULL){
            printf("Realloc failed when removing user\n");
            return;
        }
    }
    aServer->clients = temp;
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