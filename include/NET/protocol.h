#ifndef PROTOCOL_H
#define PROTOCOL_H

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