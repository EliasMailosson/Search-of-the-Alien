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

    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_AllocateChannels(16);

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
    pView->playerTexture[CHARACTER_BLUEFACE] = SDL_CreateTextureFromSurface(pView->pRend, surface);
    SDL_FreeSurface(surface);
    SDL_Surface *surface2 = IMG_Load("assets/images/player/biggie_gun.png");
    pView->playerTexture[CHARACTER_BIGGIE] = SDL_CreateTextureFromSurface(pView->pRend, surface2);
    SDL_FreeSurface(surface2);
    SDL_Surface *surface3 = IMG_Load("assets/images/player/cleopatra.png");
    pView->playerTexture[CHARACTER_CLEOPATRA] = SDL_CreateTextureFromSurface(pView->pRend, surface3);
    SDL_FreeSurface(surface3);
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
    SDL_Surface *surface8 = IMG_Load("assets/images/player/blowface_shooting.png");
    pView->playerTexture[3] = SDL_CreateTextureFromSurface(pView->pRend, surface8);
    SDL_FreeSurface(surface8);
    SDL_Surface *surface9 = IMG_Load("assets/images/player/biggie_shooting.png");
    pView->playerTexture[4] = SDL_CreateTextureFromSurface(pView->pRend, surface9);
    SDL_FreeSurface(surface9);
    SDL_Surface *surface10 = IMG_Load("assets/images/player/cleopatricia_shooting.png");
    pView->playerTexture[5] = SDL_CreateTextureFromSurface(pView->pRend, surface10);
    SDL_FreeSurface(surface10);
    SDL_Surface *surface11 = IMG_Load("assets/images/player/shadow.png");
    pView->shadowTexture = SDL_CreateTextureFromSurface(pView->pRend, surface11);
    SDL_FreeSurface(surface11);

    SDL_Surface *surface12 = IMG_Load("assets/images/enemy/enemy_bear_running.png");
    pView->enemyTexture = SDL_CreateTextureFromSurface(pView->pRend, surface12);
    SDL_FreeSurface(surface12);

    SDL_Surface *surface13 = IMG_Load("assets/images/vignette/red_vignette.png");
    pView->vignetteTexture = SDL_CreateTextureFromSurface(pView->pRend, surface13);
    SDL_FreeSurface(surface13);

    // MUSIC
    Mix_Music *backgroundMusic1 = Mix_LoadMUS("assets/music/space-whisper-long.mp3");
    pView->backgroundMusic1 = backgroundMusic1;

    Mix_Music *backgroundMusicNEMUR = Mix_LoadMUS("assets/music/marchmusic.mp3");
    pView->backgroundMusicNEMUR = backgroundMusicNEMUR;

    // SOUND FX
    Mix_Chunk *blueShot = Mix_LoadWAV("assets/sound/blueShot.mp3");
    pView->blueShot = blueShot;
    Mix_Chunk *biggieShot = Mix_LoadWAV("assets/sound/biggieShotShort.wav");
    pView->biggieShot = biggieShot;

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
    updateHudPlayerList(*aClient, pView->aHud, pView->pRend, pView->windowWidth, pView->windowHeight);

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
    Mix_FreeChunk(pView->biggieShot);
    Mix_FreeChunk(pView->blueShot);
    Mix_FreeMusic(pView->backgroundMusicNEMUR);
    Mix_FreeMusic(pView->backgroundMusic1);
    SDL_FreeCursor(pView->crosshair);
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