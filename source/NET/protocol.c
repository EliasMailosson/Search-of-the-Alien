#include "../../include/NET/protocol.h"

typedef struct stdPackage{
    GameState gameState;
    MessageType msgType;
    Uint32 payloadSize;
    Uint8 *payload; 
};

StdPackage deserializePacket(const Uint8 *buffer, Uint32 bufferSize){
    if(!buffer || bufferSize) return NULL;
    StdPackage pkg = malloc(sizeof(struct stdPackage));
    if(!pkg) return NULL;
    int offset = 0;
    pkg->gameState = SDLNet_Read32(buffer + offset);
    offset += 4;
    pkg->msgType = SDLNet_Read32(buffer + offset);
    offset += 4;
    pkg->payloadSize = SDLNet_Read32(buffer + offset);
    offset +=4;
    if(buffer < 12 + pkg->payloadSize){
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


Uint32 serializePacket(const StdPackage packet, Uint8 ** buffer){
    if(!packet || !buffer)return 0;
    Uint32 totalSize = 12 + packet->payloadSize;
    *buffer = malloc(totalSize);
    if (!*buffer) {
        fprintf(stderr, "Failed to allocate serialization buffer.\n");
        return 0;
    }
    int offset = 0;
    SDLNet_Write32(packet->gameState, *buffer + offset);
    offset += 4;
    SDLNet_Write32(packet->msgType, *buffer + offset);
    offset += 4;
    SDLNet_Write32(packet->payloadSize, *buffer + offset);
    offset += 4;
    // eventuelt att vi har en pekare tille en funton 
    // eller någor annat sätt att stoppa in paload 
    // om vi ska ha något annat en stereng ?
    if(packet->payloadSize > 0 && packet->payload){
        memcpy(*buffer + offset, packet->payload, packet->payloadSize);
    }
    return totalSize;
}

void NET_stdPakegeDestroy(StdPackage paket){
    if(paket){
        free(paket->payload);
        free(paket);
    }
}

StdPackage NET_stdPakegeCreate(GameState gameState, MessageType msgType, Uint32 payloadSize){
    StdPackage pkg = malloc(sizeof(struct stdPackage));
    if(!pkg){
        fprintf(stderr, "Failed to allocate memory for packet header.\n");
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
