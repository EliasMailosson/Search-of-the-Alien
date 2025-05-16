#ifndef CLIENT_H
#define CLIENT_H
#include <stdbool.h>
#include <stdlib.h>
#include "../../include/NET/shared.h"
#include "../../include/MAP/map.h"
#include "../../include/NET/enemies.h"
#include "../../include/sound.h"
#define SERVER_IP_PAHT "data/server_ip.config"
#define MAX_LINE 1024

enum PlayerAnimationTypes{ANIMATION_IDLE, ANIMATION_RUNNING, EMOTE_TPOSE};

typedef struct __attribute__((packed)) Proj {
    int16_t x, y;
    int8_t angle;
    uint8_t textureIdx;
}Proj;

typedef struct clientEnemy{
    SDL_Point pos;
    int direction;
    uint8_t hpEnemy;
}ClientEnemy;
struct Player{
    GameState state;
    char username[MAX_USERNAME_LEN]; //myusername
    SDL_Point pos;
    int direction;
    int currentPlayerAnimation;
    int colorIndex;
    SDL_Color color;
    int playerCharacter;
    bool isShooting;
    int dashCooldown;
    uint8_t HpProcent;
};

typedef struct WeaponStats WeaponStats;
typedef struct Player Player;
struct client;
typedef struct client *Client;
typedef struct scenC ScenC;

/** Connecting to host server via a specefic address and port */
bool NET_clientConnect(Client aClient);
/** Allocation memory for the client */
Client NET_clientCreate();
/** Freeing client memory after use and sett to NULL */
void NET_clientDestroy(Client aClient);
void NET_clientReadGraphicsConfig(Client aClient);

int NET_clientGetPlayerCount(Client aClient);
const Player* NET_clientGetPlayerList(Client aClient);
SDL_Point NET_clientGetPlayerPos(Client aClient, int playerIdx);
void NET_clientGetFriendsName(Client aClient, char outputNames[][MAX_USERNAME_LEN]);
int NET_clientGetState(Client aClient);

ClientEnemy *NET_clientGetEnemy(Client aClient, int idx);
Player *NET_clientGetPlayer(Client aClient, int idx);

void NET_clientSendInt(Client aClient,GameState GS, MessageType msgType,int placeHolder);
void NET_clientSendString(Client aClient,GameState GS, MessageType msgType,const char* str);
void NET_clientSendArray(Client aClient,GameState GS, MessageType msgType,const void* array, Uint32 arraySize);
int NET_clientGetClientState(Client aClient, int playerIdx);
bool NET_clientIsMyUsername(Client aClient, char *name);

void NET_clientSetSelfName(Client aClient, char* newName);
int NET_clientGetProjTexture(Client aClient, int projIdx);

void NET_clientSetPlayerAnimation(Client aClient, int playerIdx, int newAnimation);
int NET_clientGetPlayerAnimation(Client aClient, int playerIdx);
void NET_clientSetNextGraphicsConfig(Client aClient, int value);

void NET_clientGetPlayerName(Client aClient, int playerIndex, char* username);
int NET_clientGetPlayerDirection(Client aClient, int playerIdx);
void NET_clientGetSelfname(Client aClient, char* outputName);
int NET_clientGetSelfIndex(Client aClient);
int NET_clientGetPlayerColorIndex(Client aClient,int index);
int NET_clientIsShooting(Client aClient, int playerIdx);
bool NET_clientIsPlayerDamaged(Client aClient, int selfIndex);
int NET_clientGetGraphicsQuality(Client aClient);
int NET_clientGetNextGraphicsConfig(Client aClient);

int NET_clientGetPlayerCharacter(Client aClient, int playerIdx);
void NET_clientGetProjList(Client aClient, Proj *outputProjList);
void NET_clientUpdateProjList(Client aClient, Packet aPacket);

bool NET_clientGetTerminalHub(Client aClient);
void NET_clientReceiver(Client aClient, Map aMap,SDL_Window *pScreen, Sound aSound);

int NET_clientGetDashCooldown(Client aClient);

bool NET_clientGetPauseState(Client aClient);

int NET_clientFindPlayer(Client aClient, char* str);

void NET_clientUpdateGameState(Client aClient,Packet aPacket);
void NET_clientUpdatePlayerList(Client aClient,Packet aPacket);
//----------
SDL_Point NET_clientGetSelfPos(Client aClient);

void NET_clientSaveWeaponStats(Client aClient);
void NET_clientLoadWeaponStats(Client aClient);

SDL_Color NET_clientGetColor(int index);
SDL_Color NET_GetPlayerColor(Client aClient, int index);

int NET_clientGetHP(Client aClient, int index);

void NET_clientUpdateEnemy(Client aClient, Packet aPacket);
SDL_Point NET_clientGetEnemyPos(Client aClient, int index);
int NET_clientGetEnemyDirection(Client aClient, int index);
void NET_clientScenarioUpdate(Client aClient,uint32_t seed, Sound aSound);
int NET_clientGetEnemiesCount(Client aClinet);
int NET_clientGetEnemyHP(Client aClient, int index);
bool NET_clientIsEnemyDamaged(Client aClient, int index);


//Getters for objectives
int NET_clientGetEnemyHP(Client aClient, int index);
int NET_clientGetKillCount(Client aClient);
int NET_clientGetWaveCount(Client aClient);
ScenarioState NET_clientGetScenarioState(Client aClient);
SDL_Point NET_clientGetObjectivePoint(Client aClient);

char* NET_clientReadFileOne(char* file);


#endif