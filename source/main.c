#include "../include/game.h"
#include "../include/clientLife.h"


int main(int argc, char **argv ){
    (void)argc; (void)argv;

    ClientView view = {0};
    ClientControl control = {0};
    Client aClient = {0};
    
    startClient(&aClient, &view, &control);
    gameLoop(aClient, &control, &view);
    killClient(&aClient, &view);
    return 0;
}