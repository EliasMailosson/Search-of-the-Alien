#include "../include/clientLife.h"

SDL_Cursor* createScaledCursor(const char *filePath, int newWidth, int newHeight, int hotX, int hotY);

void startClient(Client *aClient, ClientView *pView,ClientControl *pControl){
    SDL_Init(SDL_INIT_EVERYTHING);
    pView->windowHeight = 600;
    pView->windowWidth = pView->windowHeight*16/9;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error initializing SDL: %s\n", SDL_GetError());
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        SDL_Quit();
    }
    if (TTF_Init() == -1) {
        printf("TTF_Init error: %s\n", TTF_GetError());
    }
    if (SDLNet_Init() == -1) {
        printf("Net_Init error: %s\n", SDLNet_GetError());
    }

    pView->pWin = SDL_CreateWindow("main",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,pView->windowWidth,pView->windowHeight,0);
    if (!pView->pWin) {
        printf("Error creating window: %s\n", SDL_GetError());
        SDLNet_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    pView->pRend = SDL_CreateRenderer(pView->pWin,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!pView->pRend) {
        SDL_DestroyWindow(pView->pWin);
        SDLNet_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
    pControl->isRunning = true;

    *aClient = NET_clientCreate();

    SDL_Surface *surface = IMG_Load("assets/images/player/bluethan.png");
    pView->playerTexture = SDL_CreateTextureFromSurface(pView->pRend, surface);
    SDL_FreeSurface(surface);

    pView->crosshair = createScaledCursor("assets/images/cursor/crosshair.png", 50, 50, 25, 25);

    pView->playerRenderSize = 128;

    if(!pView->crosshair){
        SDL_DestroyRenderer(pView->pRend);
        SDL_DestroyWindow(pView->pWin);
        SDLNet_Quit();
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
}

SDL_Cursor* createScaledCursor(const char *filePath, int newWidth, int newHeight, int hotX, int hotY) {
    SDL_Surface *origSurface = IMG_Load(filePath);
    if (!origSurface) {
        fprintf(stderr, "IMG_Load Error: %s\n", IMG_GetError());
        return NULL;
    }
    
    // Create a new surface with the desired dimensions.
    // We use the same pixel format as the original image.
    SDL_Surface *scaledSurface = SDL_CreateRGBSurface(0, newWidth, newHeight,
            origSurface->format->BitsPerPixel,
            origSurface->format->Rmask,
            origSurface->format->Gmask,
            origSurface->format->Bmask,
            origSurface->format->Amask);
    if (!scaledSurface) {
        fprintf(stderr, "SDL_CreateRGBSurface Error: %s\n", SDL_GetError());
        SDL_FreeSurface(origSurface);
        return NULL;
    }

    // Define the destination rectangle which covers the entire scaled surface.
    SDL_Rect destRect = { 0, 0, newWidth, newHeight };

    if (SDL_BlitScaled(origSurface, NULL, scaledSurface, &destRect) != 0) {
        fprintf(stderr, "SDL_BlitScaled Error: %s\n", SDL_GetError());
        SDL_FreeSurface(origSurface);
        SDL_FreeSurface(scaledSurface);
        return NULL;
    }
    
    SDL_Cursor* cursor = SDL_CreateColorCursor(scaledSurface, hotX, hotY);
    if (!cursor) {
        fprintf(stderr, "SDL_CreateColorCursor Error: %s\n", SDL_GetError());
    }
    
    SDL_FreeSurface(origSurface);
    SDL_FreeSurface(scaledSurface);
    
    return cursor;
}

void killClient(Client *aClient, ClientView *pView){
    SDL_FreeCursor(pView->crosshair);
    SDL_DestroyTexture(pView->playerTexture);

    NET_clientDestroy(*aClient);
    SDL_DestroyRenderer(pView->pRend);
    pView->pRend = NULL;
    SDL_DestroyWindow(pView->pWin);
    pView->pWin = NULL;
    SDLNet_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    
}
void RenderPlayerName(Client aClient, ClientView *pView, int i, SDL_Rect playerRect){
    char username[MAX_USERNAME_LEN] = {0};
    username[MAX_USERNAME_LEN - 1] = '\0';
    NET_clientGetPlayerName(aClient, i, username);

    if (strlen(username) == 0 || !pView->fonts){
        printf("%s",username);
        printf("Font: %p", (void*)pView->fonts);
        return;
    }

    SDL_Color nameColor = {255,255,255,255}; // Server ska välja färgerna på namnen
    SDL_Surface* nameSurface = TTF_RenderText_Blended(pView->fonts, username, nameColor);

    if (!nameSurface){
        printf("Andra gangen: %s", username);
        return;
    } 

    SDL_Texture* nameTexture = SDL_CreateTextureFromSurface(pView->pRend, nameSurface);
    if (!nameTexture) {
        printf("Tredje gangen: %s", username);
        SDL_FreeSurface(nameSurface);
        return;
    }

    int nameWidth = nameSurface->w;
    int nameHeight = nameSurface->h;

    SDL_Rect nameRect = {
        .x = playerRect.x + (playerRect.w - nameWidth)/2,
        .y = playerRect.y - nameHeight - 5,
        .w = nameWidth,
        .h = nameHeight
    };

    SDL_RenderCopy(pView->pRend, nameTexture, NULL, &nameRect);

    SDL_FreeSurface(nameSurface);
    SDL_DestroyTexture(nameTexture);
}