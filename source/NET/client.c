#include "../../include/NET/client.h"
#include "../../include/UI/friend.h"
struct Player{
    GameState state;
    char username[MAX_USERNAME_LEN]; //myusername
    SDL_Point pos;
    int direction;
    int currentPlayerAnimation;
    int colorIndex;
    SDL_Color color;
    int playerCharacter;
};
struct client{
    SDLNet_SocketSet socketSet;
    UDPsocket clientSocket;
    UDPpacket *pReceivePacket;
    UDPpacket *pSendPacket;
    IPaddress serverAddr;
    char selfUsername[MAX_USERNAME_LEN]; //andras usernames

    int PlayerCount;
    Player playerList[MAX_CLIENTS];
    Proj projList[MAX_CLIENT_PROJ];
}; 

bool NET_clientConnect(Client aClient){
    if (SDLNet_ResolveHost(&aClient->serverAddr,JON_IP, PORT) < 0) {
        printf("SDLNet_ResolveHost failed: %s\n", SDLNet_GetError());
        return false;
    }
    return true;
}

void NET_clientGetProjList(Client aClient, Proj *outputProjList) {
    for(int i = 0; i < MAX_CLIENT_PROJ; i++) {
        outputProjList[i].angle = aClient->projList[i].angle;
        outputProjList[i].textureIdx = aClient->projList[i].textureIdx;
        outputProjList[i].x = aClient->projList[i].x;
        outputProjList[i].y = aClient->projList[i].y;
    }
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
    aClient->PlayerCount = 1;
    aClient->playerList[0].state = MENU;
    strcpy(aClient->playerList[0].username,"None");
    strcpy(aClient->selfUsername,"None");
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

void NET_clientDestroy(Client aClient){
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

void NET_clientReceiver(Client aClient){
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
                break;
            case PRINT:
                printf("%s\n",NET_packetGetPayload(aPacket));
                break;
            case CHANGE_GAME_STATE_RESPONSE:
                NET_clientUpdateGameState(aClient,aPacket);
                break;
            case PROJ_LIST:
                NET_clientUpdateProjList(aClient, aPacket);
                break;
            default:
                printf("client recieved invalid msgType: %d!!\n", NET_packetGetMessageType(aPacket));
                break;
            }
            if(aPacket) NET_packetDestroy(aPacket);
        }
    }
}

int NET_clientGetPlayerDirection(Client aClient, int playerIdx) {
    return aClient->playerList[playerIdx].direction;
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

    }
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