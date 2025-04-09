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

void NET_serverDestroySDL(){
    SDLNet_Quit();
    SDL_Quit();
}

void NET_PlayerListUpdate(Packet aPacket, PlayerPacket* list, int *count){
    Uint8* raw = NET_packetGetPayload(aPacket);
    Uint32 size = NET_packetGetPayloadSize(aPacket);
    if(!raw){
        printf("Raw error!");
    }
    list = (PlayerPacket*)raw;
    if(!list){
        printf("list error!");
    }
    (*count) = size / sizeof(PlayerPacket);
}

void NET_PlayerListPrintf(PlayerPacket* list, int count){
    for (int i = 0; i < count; i++){
        printf("index %d, ID %s, pos X %d, pos Y %d",i,list[i].ID,list[i].pos.x,list[i].pos.y);
    }
}

void NET_PlayerListRemovePlayer(PlayerPacket **list, int index, int *listCount){
    if (index < 0 || index >= (*listCount)) {
        printf("Invalid index\n");
        return;
    }
    for (int i = index; i < (*listCount) - 1; i++) {
        (*list)[i] = (*list)[i + 1];
    }
    (*listCount)--;
    if ((*listCount) > 0) {
        PlayerPacket *temp = realloc(*list, (*listCount) * sizeof(PlayerPacket));
        if (temp == NULL) {
            printf("Realloc failed when removing PlayerPacket\n");
            return;
        }
        *list = temp;
    } else {
        free(*list);
        *list = NULL;
    }
}

void NET_PlayerListAddPlayer(PlayerPacket **list, PlayerPacket newPlayer, int *listCount){
    PlayerPacket *temp = realloc(*list, ((*listCount) + 1) * sizeof(PlayerPacket));
    if (temp != NULL) {
        *list = temp;
        (*list)[*listCount] = newPlayer;
        (*listCount)++;
    } else {
        printf("Realloc failed when adding to the PlayerPacket!\n");
    }
}

void NET_eventHandler(bool *pIsRunning, bool *pKeys,SDL_Event event){
    while (SDL_PollEvent(&event)){
        switch (event.type){
        case SDL_QUIT: *pIsRunning = false;
            break;
        case SDL_KEYDOWN: pKeys[event.key.keysym.scancode] = true;
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

// beöver fixa
// när vi gör player list (sträng)
void NET_PlayerListRead(PlayerPacket *new_player, int *playerCount){
    FILE *fp;
    fp = fopen("data/playerlist.txt", "r");
    if(fp != NULL)
    {
        fscanf(fp,"%d", &playerCount);
        for (int i = 0; i < playerCount; i++)
        {
            // fscanf(fp,"%s", username[i]);
            // fscanf(fp,"%d", playerID[i]);
        }
        
    }
    fclose(fp);
    printf("%s\n", new_player->ID);
}

void NET_PlayerListUpdateFile(int playerCount){
    FILE *fp;
    fp = fopen("data/playerlist.txt", "r");
    if (fp != NULL)
    {
      fprintf(fp,"%d\n", playerCount);
      for (int i = 0; i < playerCount; i++)
      {
        // fprintf(fp,"%s ", username[i]);
        // fprintf(fp,"%s ", playerID[i]);

        fprintf(fp,"\n");
      }
    }
    fclose(fp);
}
