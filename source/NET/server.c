#include "../../include/NET/server.h"


struct server{
    UDPsocket serverSocket;
    IPaddress clientIP[MAX_CLIENTS];
    int clientConunt;
    UDPpacket *pReceivePacket;
    UDPpacket *pSendPacket;
};

int main(int argc, char **argv ){
    (void)argc; (void)argv;
    NET_serverInitSDL();
    Server aServer = {0};
    aServer = NET_serverCreate();
    bool isRunning;
    // if Server has allocated memory then the server is running on "PORT"
    if(aServer == NULL){
        isRunning = false;
    }else{
        isRunning = true;
        printf("UDP server started on port %d\n", PORT);
    } 
    int x = 0;
    int palyerID = 0; 
    int n = 0;
    while (isRunning){
        // Poll event checking if there is a packege to recive
        while (SDLNet_UDP_Recv(aServer->serverSocket, aServer->pReceivePacket)){
            Packet aPkg = NET_packetDeserialize(aServer->pReceivePacket->data, aServer->pReceivePacket->len);
            if(aPkg == NULL){
                printf("Deserialization failed! Buffer might be invalid.\n");
                break;
            } 
            //NET_stdPakegeGetGameState(aPkg);
            switch (NET_packetGetMessageType(aPkg)){
            case CONECT:
                palyerID = (int)SDLNet_Read32(NET_packetGetPayload(aPkg));
                printf("player id %d",palyerID);
                break;
            case CONECT_RESPONSE:

                break;
            case DISCONECT:

                break;
            case DISCONECT_RESPONSE:

                break;
            case LOBBY_LIST:
                printf("Hej det är %s", (char*)(NET_packetGetPayload(aPkg)));
                break;
            case LOBBY_LIST_RESPONSE:  
                printf("hej\n");
                PlayerList list[3] = {0};
                NET_serverReceivePlayerList(aPkg,list,&n);
                NET_PlayerListPrintf(list,3);
                break;
            default:
                printf("Failed!\n");
                break;
            }
            if(aPkg) NET_packetDestroy(aPkg);
            x++;
            if(x!=0)isRunning = false;
        }
    }
    printf("%d count\n",n);

    NET_serverDestroy(aServer);
    NET_severDestroySDL();
}

void NET_serverReceivePlayerList(Packet aPkg, PlayerList* list, int *count){
    Uint8* raw = NET_packetGetPayload(aPkg);
    Uint32 size = NET_packetGetPayloadSize(aPkg);
    if(!raw){
        printf("RAWWWW error!");
    }
    list = (PlayerList*)raw;
    if(!list){
        printf("list error!");
    }
    (*count) = size / sizeof(PlayerList);
}

void NET_PlayerListPrintf(PlayerList* list, int count){
    for (int i = 0; i < count; i++){
        printf("index %d, ID %d, pos X %d, pos Y %d",i,list[i].ID,list[i].pos.x,list[i].pos.y);
    }
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
    if(aServer->serverSocket != NULL) {
        SDLNet_UDP_Close(aServer->serverSocket);
        aServer->serverSocket = NULL;
    }
    free(aServer);
}

Server NET_serverCreate(){
    Server aServer = malloc(sizeof(struct server));
    if(aServer == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }
    aServer->clientConunt = 0;
     // Open server UDP socket
    aServer->serverSocket = SDLNet_UDP_Open(PORT);
    if(!aServer->serverSocket){
        printf("Failde to open UDP socket on port %d\n",PORT);
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

