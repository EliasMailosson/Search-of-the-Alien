#include "../include/game.h"
#include "../include/clientLife.h"


int main(int argc, char **argv ){
    (void)argc; (void)argv;

    ClientView view = {0};
    ClientControl control = {0};

    startClient(&view, &control);
    gameLoop(&control, &view);
    killClient(&view);

    return 0;
}