#include "../include/clientLife.h"

#define SPRITE_SHEET_COUNT 7

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

    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_AllocateChannels(MAX_SOUND_CHANNELS);

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

    int quality = NET_clientGetGraphicsQuality(*aClient);
    char spriteSheetsHigh[SPRITE_SHEET_COUNT][64] = {
        "assets/images/player/bluethan.png", "assets/images/player/biggie_gun.png",
        "assets/images/player/cleopatra.png", "assets/images/player/blowface_shooting.png",
        "assets/images/player/biggie_shooting.png", "assets/images/player/cleopatricia_shooting.png",
        "assets/images/enemy/enemy_bear_running.png"
    };
    char spriteSheetsMedium[SPRITE_SHEET_COUNT][64] = {
        "assets/images/player/medium-res/bluethan.png", "assets/images/player/medium-res/biggie_gun.png",
        "assets/images/player/medium-res/cleopatra.png", "assets/images/player/medium-res/blowface_shooting.png",
        "assets/images/player/medium-res/biggie_shooting.png", "assets/images/player/medium-res/cleopatricia_shooting.png",
        "assets/images/enemy/medium-res/enemy_bear_running.png"
    };

    for(int i = 0; i < SPRITE_SHEET_COUNT; i++) {
        SDL_Surface *surface;
        switch(quality) {
            case 1: surface = IMG_Load(spriteSheetsHigh[i]); break;
            case 2: surface = IMG_Load(spriteSheetsMedium[i]); break;
            default: printf("Invalid graphics setting: %d\n", quality);
        }
        
        if(i == 6) {
            pView->enemyTexture = SDL_CreateTextureFromSurface(pView->pRend, surface);
            printf("loading: %s\n", spriteSheetsHigh[i]);
        } else {
            pView->playerTexture[i] = SDL_CreateTextureFromSurface(pView->pRend, surface);
        }
        SDL_FreeSurface(surface);
    }

    SDL_Surface *surface4 = IMG_Load("assets/images/Projectiles/Bullet.png");
    pView->projectileTexture[PROJ_TEX_BULLET] = SDL_CreateTextureFromSurface(pView->pRend, surface4);
    SDL_FreeSurface(surface4);
    SDL_Surface *surface5 = IMG_Load("assets/images/Projectiles/Fire projectile.png");
    pView->projectileTexture[PROJ_TEX_FIRE] = SDL_CreateTextureFromSurface(pView->pRend, surface5);
    SDL_FreeSurface(surface5);
    SDL_Surface *surface6 = IMG_Load("assets/images/Projectiles/neon blue_laser.png");
    pView->projectileTexture[PROJ_TEX_NEON_LASER] = SDL_CreateTextureFromSurface(pView->pRend, surface6);
    SDL_FreeSurface(surface6);
    SDL_Surface *surface7 = IMG_Load("assets/images/Projectiles/purple Laser.png");
    pView->projectileTexture[PROJ_TEX_PURPLE_LASER] = SDL_CreateTextureFromSurface(pView->pRend, surface7);
    SDL_FreeSurface(surface7);
    SDL_Surface *surface11 = IMG_Load("assets/images/player/shadow.png");
    pView->shadowTexture = SDL_CreateTextureFromSurface(pView->pRend, surface11);
    SDL_FreeSurface(surface11);
    SDL_Surface *surface13 = IMG_Load("assets/images/vignette/red_vignette.png");
    pView->vignetteTexture = SDL_CreateTextureFromSurface(pView->pRend, surface13);
    SDL_FreeSurface(surface13);

    pView->aSound = SOUND_create();


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
    pView->aHud = hudCreate(pView->pRend);
    updateHudPlayerList(*aClient, pView->aHud, pView->pRend, pView->windowWidth);

    pControl->selectedCharacter = CHARACTER_BLUEFACE;
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

    SOUND_destroy(pView->aSound);

    for(int i = 0; i < MAX_PLAYER_CHARACTERS; i++) {
        SDL_DestroyTexture(pView->playerTexture[i]);
        pView->playerTexture[i] = NULL;
    }
    SDL_DestroyTexture(pView->enemyTexture);
    SDL_DestroyTexture(pView->shadowTexture);

    for (int i = 0; i < MAX_PROJECTILE_TEXTURES; i++)
    {
        SDL_DestroyTexture(pView->projectileTexture[i]);
        pView->projectileTexture[i] = NULL;
    }
    SDL_DestroyTexture(pView->vignetteTexture);

    hudDestroy(pView->aHud);
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