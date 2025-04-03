#include "../../include/NET/packetHandler.h"

typedef struct packet{
    GameState gameState;
    MessageType msgType;
    Uint32 payloadSize;
    Uint8 *payload; 
}packet;


Uint32 NET_packetGetPayloadSize(Packet aPacket){
    return aPacket->payloadSize;
}

Uint32 NET_packetGetGameState(Packet aPacket){
    return aPacket->gameState;
}

Uint32 NET_packetGetMessageType(Packet aPacket){
    return aPacket->msgType;
}

Uint8* NET_packetGetPayload(Packet aPacket){
    return aPacket->payload;
}

void NET_packetWrite32Payload(Packet aPacket, Uint32 Payload){
    SDLNet_Write32(Payload, aPacket->payload);
}

void NET_packetSetPayloadString(Packet aPkg,const char* str){
    memcpy(aPkg->payload, str, strlen(str)+1);
}

void NET_packetPayloadArray(Packet aPkg, const void* array, Uint32 size){
    memcpy(aPkg->payload, array, size);
}

Packet NET_packetDeserialize(const Uint8 *buffer, Uint32 bufferSize){
    if(!buffer || bufferSize < 12) return NULL;
    Packet pkg = malloc(sizeof(struct packet));
    if(!pkg) return NULL;
    int offset = 0;
    pkg->gameState = SDLNet_Read32(buffer + offset);
    offset += 4;
    pkg->msgType = SDLNet_Read32(buffer + offset);
    offset += 4;
    pkg->payloadSize = SDLNet_Read32(buffer + offset);
    offset +=4;
    if(bufferSize < 12 + pkg->payloadSize){
        free(pkg);
        return NULL;
    }
    if(pkg->payloadSize > 0){
        pkg->payload = malloc(pkg->payloadSize);
        if(!pkg->payload){
            free(pkg);
            return NULL;
        }
        memcpy(pkg->payload, buffer + offset, pkg->payloadSize);
    }else{
        pkg->payload = NULL;
    }
    return pkg;
}


Uint32 NET_packetSerialize(const Packet aPacket, Uint8 ** buffer){
    if(!aPacket || !buffer)return 0;
    Uint32 totalSize = 12 + aPacket->payloadSize;
    *buffer = malloc(totalSize);
    if (!*buffer) {
        fprintf(stderr, "Failed to allocate serialization buffer.\n");
        return 0;
    }
    int offset = 0;
    SDLNet_Write32(aPacket->gameState, *buffer + offset);
    offset += 4;
    SDLNet_Write32(aPacket->msgType, *buffer + offset);
    offset += 4;
    SDLNet_Write32(aPacket->payloadSize, *buffer + offset);
    offset += 4;
    
    if(aPacket->payloadSize > 0 && aPacket->payload){
        memcpy(*buffer + offset, aPacket->payload, aPacket->payloadSize);
    }
    return totalSize;
}

void NET_packetDestroy(Packet Packet){
    if(Packet){
        free(Packet->payload);
        free(Packet);
    }
}

Packet NET_packetCreate(GameState gameState, MessageType msgType, Uint32 payloadSize){
    Packet pkg = malloc(sizeof(struct packet));
    if(!pkg){
        fprintf(stderr, "Failed to allocate memory for aPacket header.\n");
        return NULL;
    }
    pkg->gameState = gameState;
    pkg->msgType = msgType;
    pkg->payloadSize = payloadSize;
    pkg->payload = (payloadSize > 0) ? malloc(payloadSize) : NULL;
    if (payloadSize > 0 && !pkg->payload) {
        fprintf(stderr, "Failed to allocate memory for payload.\n");
        free(pkg);
        return NULL;
    }
    return pkg;
}