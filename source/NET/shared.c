#include "../../include/NET/shared.h"

bool NET_serverInitSDL(){
    if (SDL_Init(0) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    if (SDLNet_Init() < 0) {
        printf("SDLNet_Init failed: %s\n", SDLNet_GetError());
        SDL_Quit();
        return false;
    }
    return true;
}

void NET_severDestroySDL(){
    SDLNet_Quit();
    SDL_Quit();
}

void NET_PlayerListRead(PlayerList *new_player){
    FILE *fp;
    fp = fopen("data/playerlist.txt", "r");
    if(fp != NULL)
    {
        fscanf(fp,"%d", &new_player->ID);
    }
    fclose(fp);
    printf("%d\n", new_player->ID);
}

void NET_PlayerListUpdate(Packet aPacket, PlayerList* list, int *count){
    Uint8* raw = NET_packetGetPayload(aPacket);
    Uint32 size = NET_packetGetPayloadSize(aPacket);
    if(!raw){
        printf("Raw error!");
    }
    list = (PlayerList*)raw;
    if(!list){
        printf("list error!");
    }
    (*count) = size / sizeof(PlayerList);
}

void NET_PlayerListPrintf(PlayerList* list, int count){
    for (int i = 0; i < count; i++){
        printf("index %d, ID %d, pos X %d, pos Y %d",i,list[i].ID,list[i].pos.x,list[i].pos.y);
    }
}


void NET_PlayerListRemovPlayer(PlayerList *list,int index){
    (void)list;(void)index;
}
void NET_PlayerListAddPlayer(PlayerList *list,PlayerList new_player){
    (void)list;(void)new_player;
}

void NET_eventHandler(bool *pIsRunning, bool *pKeys[SDL_NUM_SCANCODES]){

    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type){
        case SDL_QUIT: *pIsRunning = false;
            break;
        case SDL_KEYDOWN: *pKeys[event.key.keysym.scancode] = true;
        if (pKeys[SDL_SCANCODE_ESCAPE] == true) *pIsRunning = false;
            break;
        case SDL_KEYUP: 
            pKeys[event.key.keysym.scancode] = false;
            break;
        default:
            break;
        }
    }
    
}
