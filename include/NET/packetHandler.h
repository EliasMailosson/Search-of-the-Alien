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

struct packet;
typedef struct packet *Packet;

// API functions:
Packet NET_packetDeserialize(const Uint8 *buffer, Uint32 bufferSize);
Uint32 NET_packetSerialize(const Packet packet, Uint8 ** buffer);
void NET_packetDestroy(Packet paket);
Packet NET_packetCreate(GameState gameState, MessageType msgType, Uint32 payloadSize);

Uint32 NET_packetGetPayloadSize(Packet packet);
Uint32 NET_packetGetGameState(Packet packet);
Uint32 NET_packetGetMessageType(Packet packet);
Uint8* NET_packetGetPayload(Packet packet);
void NET_packetWrite32Payload(Packet packet, Uint32 Payload);
void NET_packetSetPayloadString(Packet aPkg,const char *str);
void NET_packetPayloadArray(Packet aPkg, const void* array, Uint32 size);

#endif