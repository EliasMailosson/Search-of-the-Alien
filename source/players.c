#include <stdio.h>
#include "../include/players.h"

void renderPlayers(Client aClient, ClientView *pView) {
    int playerCount = NET_clientGetPlayerCount(aClient);
    printf("PlayerCount: %d\n", playerCount);

    SDL_Color colors[MAX_CLIENTS] = {
        {.r = 255, .g = 0, .b = 0, .a = 0},
        {.r = 0, .g = 255, .b = 0, .a = 0},
        {.r = 0, .g = 0, .b = 255, .a = 0},
        {.r = 255, .g = 255, .b = 0, .a = 0},
        {.r = 255, .g = 0, .b = 255, .a = 0},
        {.r = 0, .g = 255, .b = 255, .a = 0},
        {.r = 255, .g = 255, .b = 255, .a = 0},
        {.r = 100, .g = 100, .b = 100, .a = 0}
    };

    SDL_SetRenderDrawColor(pView->pRend, 0, 0, 50, 255);
    SDL_RenderClear(pView->pRend);
    for(int i = 0; i < playerCount; i++) {
        SDL_Point pos = NET_clientGetPlayerPos(aClient, i);
        printf("Player %d position: x:%d, y:%d\n", i, pos.x, pos.y);

        SDL_Rect playerRect = {pos.x, pos.y, 80, 80};
        SDL_SetRenderDrawColor(pView->pRend, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
        SDL_RenderFillRect(pView->pRend, &playerRect);
    }
    SDL_RenderPresent(pView->pRend);
}