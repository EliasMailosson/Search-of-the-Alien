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
    int startTime = SDL_GetTicks();
    int n = 0;
    int index = -1;
    int lobbyID = -1;
    SDL_Event event = {0};
    bool keys[SDL_NUM_SCANCODES] = {0};
    // User newUser = {0};
    while (isRunning){
        // Poll event checking if there is a packege to recive
        NET_eventHandler(&isRunning,keys,event);
        if(SDL_GetTicks() - startTime < 12000)
        while (SDLNet_UDP_Recv(aServer->serverSocket, aServer->pReceivePacket)){
            Packet aPacket = NET_packetDeserialize(aServer->pReceivePacket->data, aServer->pReceivePacket->len);
            if(aPacket == NULL){
                printf("Deserialization failed! Buffer might be invalid.\n");
                break;
            } 
            //NET_stdPakegeGetGameState(aPacket);
            switch (NET_packetGetMessageType(aPacket)){
            case CONNECT:
                NET_serverClientConnected(aPacket, aServer);
                break;
            case CONNECT_RESPONSE:
                // 
                break;
            case DISCONNECT:
                // sends string of disconnected players to everyone in lobby
                for (int i = 0; i < aServer->clientConunt; i++){
                    if (aServer->clients[i].IP.host == aServer->pReceivePacket->address.host && 
                            aServer->clients[i].IP.port == aServer->pReceivePacket->address.port ){
                            lobbyID = aServer->clients[i].LobbyID;
                            index = i;
                    }
                }
                for (int i = 0; i < aServer->clientConunt; i++){
                        if(aServer->clients[i].LobbyID == lobbyID) NET_serverSendString(aServer, GLOBAL, DISCONNECT_RESPONSE, aServer->clients[index].Username, i);
                    }
                NET_serverRemoveUser(aServer, index);
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
            case JOIN_LOBBY:
                lobbyID = (*(int*)NET_packetGetPayload(aPacket));
                for (int i = 0; i < aServer->clientConunt; i++){
                    if (aServer->clients[i].IP.host == aServer->pReceivePacket->address.host && 
                            aServer->clients[i].IP.port == aServer->pReceivePacket->address.port ){
                        aServer->clients[i].LobbyID = lobbyID;
                    }
                }
                // send loby respone with list of players in lobby
                break;
            case JOIN_LOBBY_RESPONSE:

                break;
            case PRINT:
                printf("%s\n",(char*)NET_packetGetPayload(aPacket));
                break;
            default:
                printf("Failed!\n");
                break;
            }
            if(aPacket) NET_packetDestroy(aPacket);
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

void NET_serverRemoveUser(Server aServer,int index){
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

void NET_serverClientConnected(Packet aPacket, Server aServer){
    User newUser = {0};
    newUser.Username = (char*)NET_packetGetPayload(aPacket);
    newUser.IP = aServer->pReceivePacket->address;
    newUser.LobbyID = -1;
    newUser.State = NET_packetGetMessageType(aPacket);
    NET_serverAddUser(aServer,newUser);
    NET_serverSendInt(aServer,GLOBAL,CONNECT_RESPONSE,0,aServer->clientConunt-1);
    // Test output:
    printf("username: %s\n",aServer->clients[aServer->clientConunt-1].Username);
    //
}

int NET_serverFindPlayerID(Server aServer, const char* str){
    for (int i = 0; i < aServer->clientConunt; i++){
        if(strcmp(str, aServer->clients[i].Username) == 0){
            return (i);
        }
    }
    return -1;
}





