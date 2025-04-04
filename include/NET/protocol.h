#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "../../include/NET/packetHandler.h"

void NET_protocolSendInt(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,int placeHolder);

void NET_protocolSendString(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,const char* str);

void NET_protocolSendArray(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,const void* array, Uint32 arraySize);

//########## ska ligga i shaerde #########

typedef struct{
    int ID;
    SDL_Point pos;
    GameState state;
}PlayerList1;


void NET_PlayerListRemovPlayer1(PlayerList1 **list,int index, int *listCount);
void NET_PlayerListAddPlayer1(PlayerList1 **list,PlayerList1 newPlayer,int *listCount);


#endif