#include "../../include/NET/protocol.h"

void NET_protocolConectSend(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,int playerID){
    Uint32 payloadSize = (Uint32)sizeof(playerID);
    StdPackage packet = NET_stdPackageCreate(GS,msgType,payloadSize);
    if (!packet) {
        printf("Failed to create dynamic packet.\n");
        return;
    }
    NET_stdPackageWrite32Payload(packet,(Uint32)playerID);
    // Serialize the packet.
    Uint8 *sBuffer = NULL;
    Uint32 serializedSize = NET_stdPackageSerialize(packet, &sBuffer);
    if (serializedSize == 0 || !sBuffer) {
        printf("Serialization failed.\n");
        NET_stdPackageDestroy(packet);
        return;
    }
    memcpy(pUDPpkg->data, sBuffer, serializedSize);
    pUDPpkg->len = serializedSize;
    pUDPpkg->address = IP;
    // Send the UDP packet.
    SDLNet_UDP_Send(Socket, -1, pUDPpkg);
    //free
    free(sBuffer);
    NET_stdPackageDestroy(packet);
}
