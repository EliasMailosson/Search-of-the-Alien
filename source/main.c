#include "../include/game.h"
#include "../include/clientLife.h"
#include "../include/NET/client.h"
#include "../include/NET/shared.h"


int main(int argc, char **argv ){
    (void)argc; (void)argv;

    ClientView view = {0};
    ClientControl control = {0};

    startClient(&view, &control);
    //gameLoop(&control, &view);
    Client aClient = {0};
    aClient = NET_clientCreate();
    NET_clientConnect(aClient);
    //NET_clientSend(aClient);
    //NET_clientSendString(aClient, MENU, LOBBY_LIST, "Elias Mailadasson");
    //PlayerList placeholder[3] = {{.ID = 6543, .pos.x = 12, .pos.y = 55}, {.ID = 1235, .pos.x = 52, .pos.y = 35}, {.ID = 6435, .pos.x = 1, .pos.y = 2}};
    //NET_clientSendArray(aClient, MENU, LOBBY_LIST_RESPONSE, placeholder, sizeof(placeholder));
    bool isRunning = true;
    bool keys[SDL_NUM_SCANCODES] = {false};
    NET_clientSendString(aClient,MENU,CONNECT,"Caspar");
    int startTime = SDL_GetTicks();
    SDL_Event event ={0};
    while (isRunning){
        NET_clientReceiver(aClient);
        while (SDL_PollEvent(&event)){
        switch (event.type){
        case SDL_QUIT: isRunning = false;
            break;
        case SDL_KEYDOWN: keys[event.key.keysym.scancode] = true;

            break;
        case SDL_KEYUP: 
            keys[event.key.keysym.scancode] = false;
            break;
        default:
            break;
        }
    } 
        if (keys[SDL_SCANCODE_ESCAPE] == true) isRunning = false;
        if(keys[SDL_SCANCODE_P]){
            NET_clientSendString(aClient,MENU,PRINT,"ndndndnslkks jklds");
            keys[SDL_SCANCODE_P] = false;
        } 
    }
    NET_clientSendString(aClient,MENU,DISCONNECT,"hej hej");
    NET_clientDestroy(aClient);
    killClient(&view);
    return 0;
}