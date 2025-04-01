#include "../../include/NET/client.h"
#include "../../include/NET/shared.h"
struct client{
    UDPsocket clientSocket;
    UDPpacket *pReceivePacket;
    UDPpacket *pSendPacket;
    int clientId;
    IPaddress serverAddr;
}; 


void NET_clientSend(Client aClient){
    Uint8 buffer[20];
    SDLNet_Write32((Uint32)2,buffer);
    SDLNet_Write32((Uint32)2,buffer + 4);
    SDLNet_Write32((Uint32)2,buffer + 8);
    memcpy(aClient->pSendPacket->data, buffer, sizeof(buffer));
    aClient->pSendPacket->len = sizeof(buffer);
    aClient->pSendPacket->address = aClient->serverAddr;
    SDLNet_UDP_Send(aClient->clientSocket, -1, aClient->pSendPacket);
}


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
    return aClient;
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