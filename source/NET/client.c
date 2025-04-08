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
    char* clientId;
    IPaddress serverAddr;
    
    PlayerPacket *list; //temporary
    int PlayerCount;
    GameState state; // rm
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
    aClient->PlayerCount = 0;
    aClient->list = NULL;
    aClient->state = MENU;
    return aClient;
}

int NET_clientGetState(Client aClient) {
    return aClient->state;
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
    char* playerID = 0;
    int index = 0;
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
            playerID = (char*)NET_packetGetPayload(aPacket);
            index = NET_clientFindPlayer(aClient,playerID);
            NET_PlayerListRemovePlayer(&aClient->list,index,&aClient->PlayerCount);
            break;
        case JOIN_LOBBY_RESPONSE:
            NET_PlayerListUpdate(aPacket,aClient->list,&aClient->PlayerCount);
            break;
        case LOBBY_LIST:
            // clientside uppdat player list
            break;
        case PRINT:
            printf("%s\n",NET_packetGetPayload(aPacket));
        default:
            break;
        }
        if(aPacket) NET_packetDestroy(aPacket);
    }
}

int NET_clientFindPlayer(Client aClient, char* str){
    for (int i = 0; i < aClient->PlayerCount; i++){
        if(strcmp(str,aClient->list[i].ID)){
            return i;
        }   
    }
    return -1;
}