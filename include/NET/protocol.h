#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "../../include/NET/packetHandler.h"
#include <stdbool.h>

#define MAX_USERNAME_LEN 40
typedef struct PlayerPacket{
    char username[MAX_USERNAME_LEN];
    SDL_Point pos;
    GameState state;
}PlayerPacket;  

void NET_protocolSendInt(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,int placeHolder);

void NET_protocolSendString(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,const char* str);

void NET_protocolSendArray(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,const void* array, Uint32 arraySize);

void NET_playerPacketReceive(Packet aPacket, PlayerPacket *list, int *count);


#endif