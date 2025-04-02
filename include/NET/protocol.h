#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdio.h>
#include <string.h>
#include <SDL_net.h>
union {
    float f;
    Uint32 u;
} floatConv;

typedef enum {
    MSG_CONECT = 0,      
    MSG_DISCONECT = 1  
} MessageType;

typedef struct {
    int gameState;
    int messageType;  // One of MessageType
    int playerID;     // Which player is sending
} PacketData;


#endif