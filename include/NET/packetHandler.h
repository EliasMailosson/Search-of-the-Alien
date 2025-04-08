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
    CONNECT,  
    CONNECT_RESPONSE,
    DISCONNECT,
    DISCONNECT_RESPONSE,
    LOBBY_LIST,
    LOBBY_LIST_RESPONSE,
    JOIN_LOBBY,
    JOIN_LOBBY_RESPONSE,
    PRINT,
    CHANGE_GAME_STATE,
    CHANGE_GAME_STATE_RESPONSE,
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
Uint32 NET_packetSerialize(const Packet aPacket, Uint8 ** buffer);
void NET_packetDestroy(Packet paket);
Packet NET_packetCreate(GameState gameState, MessageType msgType, Uint32 payloadSize);

Uint32 NET_packetGetPayloadSize(Packet aPacket);
Uint32 NET_packetGetGameState(Packet aPacket);
Uint32 NET_packetGetMessageType(Packet aPacket);
Uint8* NET_packetGetPayload(Packet aPacket);
void NET_packetWrite32Payload(Packet aPacket, Uint32 Payload);
void NET_packetSetPayloadString(Packet aPacket,const char *str);
void NET_packetPayloadArray(Packet aPacket, const void* array, Uint32 size);

#endif