#include "../../include/NET/server.h"
struct Player{
    SDL_Rect hitBox;
};

struct User{
    IPaddress IP;
    char* username;
    int LobbyID;
    GameState State;
    Player player;
};
struct server{
    UDPsocket serverSocket;
    int clientCount;
    UDPpacket *pReceivePacket;
    UDPpacket *pSendPacket;
    User *clients;
    bool isOff;
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
    while (isRunning){
        while (SDLNet_UDP_Recv(aServer->serverSocket, aServer->pReceivePacket)){
            Packet aPacket = NET_packetDeserialize(aServer->pReceivePacket->data, aServer->pReceivePacket->len);
            if(aPacket == NULL){
                printf("Deserialization failed! Buffer might be invalid.\n");
                break;
            } 
            switch (NET_packetGetMessageType(aPacket)){
            case CONNECT:
                NET_serverClientConnected(aPacket, aServer);
                NET_serverSendPlayerPacket(aServer);
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
                NET_serverSendString(aServer,GLOBAL,PRINT,"nej va fan\n",NET_serverCompIP(aServer));
                break;
            case CHANGE_GAME_STATE:
                NET_serverChangeGameStateOnClient(aServer, aPacket);
                break;
            default:
                printf("Failed!\n");
                break;
            }
            if(aPacket) NET_packetDestroy(aPacket);
        }
        if(aServer->isOff)break;
    }
    NET_serverDestroy(aServer);
    NET_serverDestroySDL();
    return 0;
}

void NET_serverSendPlayerPacket(Server aServer){
    int count = aServer->clientCount;
    PlayerPacket packet[MAX_CLIENTS];

    for (int i = 0; i < count; i++){
        packet[i].state = aServer->clients[i].State;
        packet[i].username = aServer->clients[i].username;
        SDL_Point pos = { 
            .x = aServer->clients[i].player.hitBox.x, 
            .y = aServer->clients[i].player.hitBox.y 
        };
        packet[i].pos = pos;
    }
    Uint32 payloadSize = count * sizeof(PlayerPacket);

    for (int i = 0; i < count; i++){
        NET_serverSendArray(aServer, GLOBAL, LOBBY_LIST, packet, payloadSize, i);
    }
}

void NET_serverChangeGameStateOnClient(Server aServer,Packet aPacket){
    int newState = *(int*)NET_packetGetPayload(aPacket);
    NET_serverSendInt(aServer,GLOBAL,CHANGE_GAME_STATE_RESPONSE,newState,NET_serverCompIP(aServer));
    printf("username: %s gameState is now %d\n",aServer->clients[NET_serverCompIP(aServer)].username,newState);
}

void NET_serverClientDisconnect(Server aServer){
    int lobbyID = aServer->clients[NET_serverCompIP(aServer)].LobbyID;
    for (int i = 0; i < aServer->clientCount; i++){
        if(aServer->clients[i].LobbyID == lobbyID){
            NET_serverSendPlayerPacket(aServer);
            //NET_serverSendString(aServer, GLOBAL, DISCONNECT_RESPONSE, aServer->clients[NET_serverCompIP(aServer)].Username, i);
        } 
    }
    printf("username: %s disconnected to server\n",aServer->clients[NET_serverCompIP(aServer)].username);
    NET_serverRemoveUser(aServer, NET_serverCompIP(aServer));
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
    aServer->clientCount = 0;
    aServer->isOff = false;
     // Open server UDP socket
    aServer->serverSocket = SDLNet_UDP_Open(PORT);
    if(!aServer->serverSocket){
        printf("Failed to open UDP socket on port %d\n",PORT);
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
    // Make a copy of the username string to avoid use-after-free
    char *usernameFromPacket = (char*)NET_packetGetPayload(aPacket);
    newUser.username = strdup(usernameFromPacket);
    if(newUser.username == NULL){
        fprintf(stderr, "Failed to allocate memory for username\n");
        return;
    }
    newUser.IP = aServer->pReceivePacket->address;
    newUser.LobbyID = -1;
    newUser.State = NET_packetGetMessageType(aPacket);
    
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