#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "../../include/NET/packetHandler.h"

void NET_protocolConectSend(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,int playerID);


#endif