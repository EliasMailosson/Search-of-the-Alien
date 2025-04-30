#include "../../include/NET/protocol.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static bool playerLastMousePosCheck(int x, int y);

void NET_playerPacketReceive(Packet aPacket, PlayerPacket *list, int *count){
    Uint8* raw = NET_packetGetPayload(aPacket);
    Uint32 size = NET_packetGetPayloadSize(aPacket);

    if (!raw) {
        printf("Raw error!\n");
        return;
    }

    int playerCount = size / sizeof(PlayerPacket);
    memcpy(list, raw, playerCount * sizeof(PlayerPacket));
    *count = playerCount;
}

void NET_enemyPacketReceive(Packet aPacket, EnemyPacket *enemyData){
    Uint8* raw = NET_packetGetPayload(aPacket);
    Uint32 size = NET_packetGetPayloadSize(aPacket);

    if (!raw) {
        printf("Raw error!\n");
        return;
    }
    //maybe?
    //memcpy(enemyData, raw, size);
    int enemyCount = size / sizeof(EnemyPacket);
    memcpy(enemyData, raw, enemyCount * sizeof(EnemyPacket));
}

void NET_projPacketReceive(Packet aPacket, ProjPacket *list, int *count){
    Uint8* raw = NET_packetGetPayload(aPacket);
    Uint32 size = NET_packetGetPayloadSize(aPacket);

    if (!raw) {
        printf("Raw error!\n");
        return;
    }

    int listCount = size / sizeof(ProjPacket);
    memcpy(list, raw, listCount * sizeof(ProjPacket));
    *count = listCount;
}

void NET_protocolSendInt(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,int placeHolder){
    Uint32 payloadSize = (Uint32)sizeof(placeHolder);
    Packet packet = NET_packetCreate(GS,msgType,payloadSize);
    if (!packet) {
        printf("Failed to create dynamic packet.\n");
        return;
    }
    NET_packetWrite32Payload(packet,(Uint32)placeHolder);
    // Serialize the packet.
    Uint8 *sBuffer = NULL;
    Uint32 serializedSize = NET_packetSerialize(packet, &sBuffer);
    if (serializedSize == 0 || !sBuffer) {
        printf("Serialization failed.\n");
        NET_packetDestroy(packet);
        return;
    }
    memcpy(pUDPpkg->data, sBuffer, serializedSize);
    pUDPpkg->len = serializedSize;
    pUDPpkg->address = IP;
    // Send the UDP packet.
    SDLNet_UDP_Send(Socket, -1, pUDPpkg);
    //free
    free(sBuffer);
    NET_packetDestroy(packet);
}

void NET_protocolSendString(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,const char* str){
    Uint32 payloadSize = (Uint32)(strlen(str) + 1);
    Packet packet = NET_packetCreate(GS,msgType,payloadSize);
    if (!packet) {
        printf("Failed to create dynamic packet.\n");
        return;
    }
    NET_packetSetPayloadString(packet, str);
    // Serialize the packet.
    Uint8 *sBuffer = NULL;
    Uint32 serializedSize = NET_packetSerialize(packet, &sBuffer);
    if (serializedSize == 0 || !sBuffer) {
        printf("Serialization failed.\n");
        NET_packetDestroy(packet);
        return;
    }
    memcpy(pUDPpkg->data, sBuffer, serializedSize);
    pUDPpkg->len = serializedSize;
    pUDPpkg->address = IP;
    // Send the UDP packet.
    SDLNet_UDP_Send(Socket, -1, pUDPpkg);
    //free
    free(sBuffer);
    NET_packetDestroy(packet);
}

void NET_protocolSendArray(UDPpacket *pUDPpkg, UDPsocket Socket, IPaddress IP,
                           GameState GS, MessageType msgType, const void* array, Uint32 arraySize){
    Uint32 payloadSize = arraySize;
    if (!array || payloadSize == 0) {
        printf("Invalid array data.\n");
        return;
    }
    Packet packet = NET_packetCreate(GS, msgType, payloadSize);
    if (!packet) {
        printf("Failed to create dynamic packet.\n");
        return;
    }
    NET_packetPayloadArray(packet, array, arraySize);
    Uint8 *sBuffer = NULL;
    Uint32 serializedSize = NET_packetSerialize(packet, &sBuffer);
    if (serializedSize == 0 || !sBuffer) {
        printf("Serialization failed.\n");
        NET_packetDestroy(packet);
        return;
    }
    memcpy(pUDPpkg->data, sBuffer, serializedSize);
    pUDPpkg->len = serializedSize;
    pUDPpkg->address = IP;
    // Send the UDP packet.
    SDLNet_UDP_Send(Socket, -1, pUDPpkg);
    // Clean up.
    free(sBuffer);
    NET_packetDestroy(packet);
}

bool NET_playerInputPacketCheck(PlayerInputPacket pip){
    for (int i = 0; i < NUM_PLAYER_INPUTS; i++){
        if(pip.keys[i]) return true;
        else if(playerLastMousePosCheck(pip.mousePos.x, pip.mousePos.y)) 
            return true;
    }
    return false;
}

static bool playerLastMousePosCheck(int x, int y){
    static int lastX = -1;
    static int lastY = -1;

    if (x != lastX || y != lastY) {
        lastX = x;
        lastY = y;
        return true;
    }
    return false;
}