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

// beöver fixa
// när vi gör player list (sträng)
void NET_PlayerListRead(char* selfUsername){
    FILE *fp;
    fp = fopen("data/playerlist.txt", "r");
    if(fp != NULL)
    {
        fscanf(fp,"%c", selfUsername);
    }
    fclose(fp);
    printf("%s\n", selfUsername);
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
