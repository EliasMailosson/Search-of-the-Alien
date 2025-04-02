#include "../../include/NET/server.h"
#include "../../include/NET/shared.h"
#include "../../include/NET/protocol.h"

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
    while (isRunning){
        // Poll event checking if there is a packege to recive
        while (SDLNet_UDP_Recv(aServer->serverSocket, aServer->pReceivePacket)){
            StdPackage aPkg = deserializePacket(aServer->pReceivePacket->data, aServer->pReceivePacket->len);
            
            if(aPkg){
                printf("GS: %u\n", NET_stdPakegeGettGS(aPkg));
                printf("MSG: %u\n", NET_stdPakegeGettMSG(aPkg));
                printf("PL: %u\n", NET_stdPakegeGettPL(aPkg));

                NET_stdPakegeDestroy(aPkg);
            }
            else{
                printf("Failed!\n");
            }

            x++;
            if(x!=0)isRunning = false;
        }
    }
    NET_serverDestroy(aServer);
    NET_severDestroySDL();
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

