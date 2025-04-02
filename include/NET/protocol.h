#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL_net.h>
union {
    float f;
    Uint32 u;
} floatConv;

typedef enum{
    CONECT,  
    CONECT_RESPONSE,
    DISCONECT,
    DISCONECT_RESPONSE,
    PLAYER_LIST,
    PLAYER_LIST_RESPONSE,
} MessageType;


typedef enum{
    GLOBAL,
    MENU,
    LOBBY
}GameState;

typedef struct{
    int gameState;
    int messageType;  // One of MessageType
    int playerID;     // Which player is sending
} PacketData;

struct stdPackage;
typedef struct stdPackage *StdPackage;

// API functions:
StdPackage NET_stdPackageDeserialize(const Uint8 *buffer, Uint32 bufferSize);
Uint32 NET_stdPackageSerialize(const StdPackage packet, Uint8 ** buffer);
void NET_stdPakegeDestroy(StdPackage paket);
StdPackage NET_stdPakegeCreate(GameState gameState, MessageType msgType, Uint32 payloadSize);

Uint32 NET_stdPakegeGetGameState(StdPackage packet);
Uint32 NET_stdPakegeGetMessageType(StdPackage packet);
Uint8* NET_stdPakegeGetPayload(StdPackage packet);
void NET_stdPakegeWrite32Payload(StdPackage packet, Uint32 Payload);
void NET_stdPakegeSetPayloadString(StdPackage aPkg,const char* str);


#endif