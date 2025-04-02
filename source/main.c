#include "../include/game.h"
#include "../include/clientLife.h"
#include "../include/NET/client.h"


int main(int argc, char **argv ){
    (void)argc; (void)argv;

    ClientView view = {0};
    ClientControl control = {0};

    startClient(&view, &control);
    gameLoop(&control, &view);
    Client aClient = {0};
    aClient = NET_clientCreate();
    NET_clientConnect(aClient);
    NET_clientSend(aClient);
    NET_clientDestroy(aClient);
    killClient(&view);
    return 0;
}