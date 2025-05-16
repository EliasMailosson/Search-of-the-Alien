#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "../../include/NET/packetHandler.h"
#include <stdbool.h>

#define MAX_USERNAME_LEN 40
#define MAX_CLIENT_PROJ 32

enum ProjectileTextures{PROJ_TEX_NONE, PROJ_TEX_BULLET, PROJ_TEX_FIRE, PROJ_TEX_NEON_LASER, PROJ_TEX_PURPLE_LASER};

typedef struct __attribute__((packed)) projPacket {
    int16_t x, y;
    int8_t angle;
    uint8_t textureIdx;
}ProjPacket;
typedef struct PlayerPacket{
    char username[MAX_USERNAME_LEN];
    SDL_Point pos;
    GameState state;
    SDL_Point mousePos;
    int direction;
    int colorIndex;
    int playerCharacter;
    int dashCoolDown;
    bool isShooting;
    uint8_t HpProcent;

    uint8_t xp;
    uint8_t lvl;
}PlayerPacket;  

typedef struct __attribute__((packed)) enemyPacket{
    int16_t x, y, direction;
    uint8_t hp;
}EnemyPacket;  

void NET_protocolSendInt(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,int placeHolder);

void NET_protocolSendString(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,const char* str);

void NET_protocolSendArray(UDPpacket *pUDPpkg,UDPsocket Socket,IPaddress IP,
                            GameState GS,MessageType msgType,const void* array, Uint32 arraySize);

void NET_playerPacketReceive(Packet aPacket, PlayerPacket *list, int *count);
void NET_projPacketReceive(Packet aPacket, ProjPacket *list, int *count);

void NET_enemyPacketReceive(Packet aPacket, EnemyPacket *enemyData, int *pEnemyCount);

bool NET_playerInputPacketCheck(PlayerInputPacket pip);




#endif
