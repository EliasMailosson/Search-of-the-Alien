#include "../../include/NET/client.h"

struct Player{
    GameState state;
    char* username;
    SDL_Point pos;
};
struct client{
    UDPsocket clientSocket;
    UDPpacket *pReceivePacket;
    UDPpacket *pSendPacket;
    char* selfUsername;
    IPaddress serverAddr;
    
    PlayerPacket playerPaket; //to send your pos
    int PlayerCount;
    Player playerList[MAX_CLIENTS];
}; 

bool NET_clientConnect(Client aClient){
    if (SDLNet_ResolveHost(&aClient->serverAddr,JON_IP, PORT) < 0) {
        printf("SDLNet_ResolveHost failed: %s\n", SDLNet_GetError());
        return false;
    }
    return true;
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
    aClient->playerList[0].username ="Caspar";
    aClient->selfUsername ="Caspar";
    return aClient;
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

SDL_Point NET_clientGetPlayerPos(Client aClient, int playerIdx) {
    if(playerIdx < MAX_CLIENTS) {
        return aClient->playerList->pos;
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
    while (SDLNet_UDP_Recv(aClient->clientSocket, aClient->pReceivePacket)){
        Packet aPacket = NET_packetDeserialize(aClient->pReceivePacket->data, aClient->pReceivePacket->len);
        if(aPacket == NULL){
            printf("Deserialization for Client failed! Buffer might be invalid.\n");
            break;
        } 
        //Handle recieving packets from the server
        switch (NET_packetGetMessageType(aPacket)){
        case CONNECT_RESPONSE:
            printf("received %d",*((int*)NET_packetGetPayload(aPacket)));
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
        default:
            break;
        }
        if(aPacket) NET_packetDestroy(aPacket);
    }
}

void NET_clientUpdatePlayerList(Client aClient,Packet aPacket){
    PlayerPacket packet[MAX_CLIENTS] = {0};
    NET_playerPacketReceive(aPacket,packet,&aClient->PlayerCount);
    for (int i = 0; i < aClient->PlayerCount; i++){
        if(packet[i].pos.y == 0){
            aClient->PlayerCount = packet->pos.x;
            break;
        } 
        aClient->playerList[i].pos = packet[i].pos;
        aClient->playerList[i].state = packet[i].state;
        aClient->playerList[i].username = packet[i].username;
    }
    // print for test
    for (int i = 0; i < aClient->PlayerCount; i++){
        printf("index %d: usernam %s: state %d:",i,aClient->playerList[i].username,aClient->playerList[i].state);
    }
}

void NET_clientUpdateGameState(Client aClient,Packet aPacket){
    int newState = *(int*)NET_packetGetPayload(aPacket);
    aClient->playerList[NET_clientFindPlayer(aClient,aClient->selfUsername)].state = newState;
}

int NET_clientFindPlayer(Client aClient, char* str){
    for (int i = 0; i < aClient->PlayerCount; i++){
        if(strcmp(str,aClient->playerList[i].username)){
            return i;
        }   
    }
    return -1;
}