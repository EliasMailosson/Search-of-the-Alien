#ifndef PACKETHANDLER_H
#define PACKETHANDLER_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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
    PLAYER_INPUT,
    ENEMY_POS,
    NEW_SEED,
    PROJ_LIST,
} MessageType;

typedef enum{
    GLOBAL,
    MENU,
    LOBBY,
    NEMUR,
    AURANTIC,
    CINDORA
}GameState;

#define NUM_PLAYER_INPUTS 7
enum PlayerInput{PLAYER_INPUT_UP, PLAYER_INPUT_DOWN, PLAYER_INPUT_RIGHT, PLAYER_INPUT_LEFT, PLAYER_INPUT_CHOOSE_PLANET, PLAYER_INPUT_SPACE, PLAYER_INPUT_MOUSEDOWN, PLAYER_INPUT_MOUSEUP};
enum PlayerCharacters{CHARACTER_BLUEFACE, CHARACTER_BIGGIE, CHARACTER_CLEOPATRA};

typedef struct playerInputPacket {
    bool keys[NUM_PLAYER_INPUTS];
    SDL_Point mousePos;
    int selecterPlayerCharacter;
} PlayerInputPacket;

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