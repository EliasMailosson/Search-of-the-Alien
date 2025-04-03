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