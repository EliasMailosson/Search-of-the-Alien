#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL_net.h>
// union {
//     float f;
//     Uint32 u;
// } floatConv;

typedef enum{
    CONECT,  
    CONECT_RESPONSE,
    DISCONECT,
    DISCONECT_RESPONSE,
    LOBBY_LIST,
    LOBBY_LIST_RESPONSE,
} MessageType;


typedef enum{
    GLOBAL,
    MENU,
    LOBBY
}GameState;

struct stdPackage;
typedef struct stdPackage *StdPackage;

// API functions:
StdPackage NET_stdPackageDeserialize(const Uint8 *buffer, Uint32 bufferSize);
Uint32 NET_stdPackageSerialize(const StdPackage packet, Uint8 ** buffer);
void NET_stdPackageDestroy(StdPackage paket);
StdPackage NET_stdPackageCreate(GameState gameState, MessageType msgType, Uint32 payloadSize);

Uint32 NET_stdPackageGetPayloadSize(StdPackage packet);
Uint32 NET_stdPackageGetGameState(StdPackage packet);
Uint32 NET_stdPackageGetMessageType(StdPackage packet);
Uint8* NET_stdPackageGetPayload(StdPackage packet);
void NET_stdPackageWrite32Payload(StdPackage packet, Uint32 Payload);
void NET_stdPackageSetPayloadString(StdPackage aPkg,const char *str);
void NET_stdPackagePayloadArray(StdPackage aPkg, const void* array);

#endif