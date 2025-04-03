#include "../../include/NET/server.h"

struct User{
    IPaddress IP;
    char* Username;
    int LobbyID;
    GameState State;
};
struct server{
    UDPsocket serverSocket;
    int clientConunt;
    UDPpacket *pReceivePacket;
    UDPpacket *pSendPacket;
    User *clients;
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
    int n = 0;
    User newUser = {0};
    while (isRunning){
        // Poll event checking if there is a packege to recive
        while (SDLNet_UDP_Recv(aServer->serverSocket, aServer->pReceivePacket)){
            Packet aPacket = NET_packetDeserialize(aServer->pReceivePacket->data, aServer->pReceivePacket->len);
            if(aPacket == NULL){
                printf("Deserialization failed! Buffer might be invalid.\n");
                break;
            } 
            //NET_stdPakegeGetGameState(aPacket);
            switch (NET_packetGetMessageType(aPacket)){
            case CONNECT:
                newUser = (User){0};
                newUser.Username = (char*)NET_packetGetPayload(aPacket);
                newUser.IP = aServer->pReceivePacket->address;
                newUser.LobbyID = -1;
                newUser.State = NET_packetGetMessageType(aPacket);
                NET_serverAddUser(aServer,newUser);
                NET_serverSendInt(aServer,GLOBAL,CONNECT_RESPONSE,0,aServer->clientConunt-1);
                //test
                printf("usernam: %s\n",aServer->clients[aServer->clientConunt-1].Username);
                break;
            case CONNECT_RESPONSE:
                // 
                break;
            case DISCONNECT:
                // fixa funtion som sökret ingeon clients och retunerar inxex
                break;
            case DISCONNECT_RESPONSE:
                //brodcast till lobbyID
                break;
            case LOBBY_LIST:
                printf("Hej det är %s", (char*)(NET_packetGetPayload(aPacket)));
                break;
            case LOBBY_LIST_RESPONSE:  
                printf("hej\n");
                PlayerList list[3] = {0};
                NET_PlayerListUpdate(aPacket,list,&n);
                NET_PlayerListPrintf(list,3);
                break;
            default:
                printf("Failed!\n");
                break;
            }
            if(aPacket) NET_packetDestroy(aPacket);
            x++;
            if(x!=0)isRunning = false;
        }
    }
    printf("%d count\n",n);

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
    free(aServer->clients);
    free(aServer);
}

Server NET_serverCreate(){
    Server aServer = malloc(sizeof(struct server));
    if(aServer == NULL){
        fprintf(stderr,"Error allocating memory for server\n");
        return NULL;
    }
    aServer->clients = NULL;
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

void NET_serverRemovUser(Server aServer,int index){
    if(index < 0 || index >= aServer->clientConunt){
        printf("Invalid index\n");
        return;
    }

    for (int i = index; i < aServer->clientConunt - 1; i++){
        aServer->clients[i] = aServer->clients[i + 1];
    }

    User* temp = NULL;
    if(aServer->clientConunt - 1 > 0){
        temp = realloc(aServer->clients, (aServer->clientConunt - 1) * sizeof(User));
        if(temp == NULL){
            printf("Realloc failed when removing user\n");
            return;
        }
    }
    aServer->clients = temp;
    aServer->clientConunt--;
}

void NET_serverAddUser(Server aServer, User newUser){
    User* temp = realloc(aServer->clients, (aServer->clientConunt + 1) * sizeof(User));

    if(temp != NULL){
        aServer->clients = temp;
        aServer->clients[aServer->clientConunt] = newUser;
        aServer->clientConunt++;
    }
    else{
        printf("Realloc failed when adding a user!\n");
    }
}




