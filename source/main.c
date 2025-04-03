#include "../include/game.h"
#include "../include/clientLife.h"
#include "../include/NET/client.h"


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
    NET_clientSendString(aClient,MENU,CONNECT,"Caspar");
    int startTime = SDL_GetTicks();
    while (SDL_GetTicks() - startTime < 5000 ){
        NET_clientReceiver(aClient);
    }
    
    NET_clientDestroy(aClient);
    killClient(&view);
    return 0;
}