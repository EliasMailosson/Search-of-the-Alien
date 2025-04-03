#include "../../include/NET/protocol.h"

void NET_protocolSendInt(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,int placeHolder){
    Uint32 payloadSize = (Uint32)sizeof(placeHolder);
    StdPackage packet = NET_stdPackageCreate(GS,msgType,payloadSize);
    if (!packet) {
        printf("Failed to create dynamic packet.\n");
        return;
    }
    NET_stdPackageWrite32Payload(packet,(Uint32)placeHolder);
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

void NET_protocolSendString(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,const char* str){
    Uint32 payloadSize = (Uint32)(strlen(str) + 1);
    StdPackage packet = NET_stdPackageCreate(GS,msgType,payloadSize);
    if (!packet) {
        printf("Failed to create dynamic packet.\n");
        return;
    }
    NET_stdPackageSetPayloadString(packet, str);
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

void NET_protocolSendArray(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
    GameState GS,MessageType msgType, const void* placeHolder, Uint32 arraySize){
    
    Uint32 payloadSize = (Uint32)sizeof(placeHolder);
    if (!placeHolder || payloadSize == 0) {
        printf("Invalid array data.\n");
        return;
    }
    
    StdPackage packet = NET_stdPackageCreate(GS,msgType,payloadSize);
    if (!packet) {
        printf("Failed to create dynamic packet.\n");
        return;
    }

    NET_stdPackagePayloadArray(packet, placeHolder, arraySize);
    //NET_stdPackageWrite32Payload(packet,(Uint32*)placeHolder);
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