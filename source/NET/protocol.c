#include "../../include/NET/protocol.h"

void NET_clientConect(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,int playerID){
    Uint32 payloadSize = (Uint32)sizeof(playerID);
    StdPackage packet = NET_stdPakegeCreate(MENU,CONECT,payloadSize);
    if (!packet) {
        printf("Failed to create dynamic packet.\n");
        return;
    }
    NET_stdPakegeWrite32Payload(packet,(Uint32)playerID);
    // Serialize the packet.
    Uint8 *sBuffer = NULL;
    Uint32 serializedSize = serializePacket(packet, &sBuffer);
    if (serializedSize == 0 || !sBuffer) {
        printf("Serialization failed.\n");
        NET_stdPakegeDestroy(packet);
        return;
    }
    memcpy(pUDPpkg->data, sBuffer, serializedSize);
    pUDPpkg->len = serializedSize;
    pUDPpkg->address = IP;
    // Send the UDP packet.
    SDLNet_UDP_Send(Socket, -1, pUDPpkg);
    //free
    free(sBuffer);
    NET_stdPakegeDestroy(packet);
}

int NET_serverConectRespons(){
    
}