#include "../include/clientLife.h"
#include "../include/sound.h"

Sound SOUND_create(void) {
    Sound aSound = malloc(sizeof(*aSound));
    if (!aSound) {
        fprintf(stderr, "Error allocating Sound struct\n");
        return NULL;
    }
    aSound->currentTrack = MUSIC_NONE;
    aSound->masterVolume = 128;
    for (int i = 0; i < MAX_SOUND_CHANNELS; i++) {
        aSound->currentChannel[i] = -1;
    }

    // MUSIC
    Mix_Music *musicMENU = Mix_LoadMUS("assets/music/space-whisper-long.wav");
    aSound->musicMENU = musicMENU;
    Mix_Music *musicNEMUR = Mix_LoadMUS("assets/music/marchmusic.wav");
    aSound->musicNEMUR = musicNEMUR;

    // SOUND FX
    Mix_Chunk *dashSound = Mix_LoadWAV("assets/sound/dashSound.wav");
    aSound->dashSound = dashSound;

    Mix_Chunk *blueShot = Mix_LoadWAV("assets/sound/blueShot.mp3");
    aSound->blueShot = blueShot;

    Mix_Chunk *cleoShot = Mix_LoadWAV("assets/sound/cleoShot.wav");
    aSound->cleoShot = cleoShot;

    Mix_Chunk *biggieShotLoop = Mix_LoadWAV("assets/sound/biggieShotLong.wav");
    aSound->biggieShotLoop = biggieShotLoop;

    Mix_Chunk *biggiesShots[4] = {NULL};
    biggiesShots[0] = Mix_LoadWAV("assets/sound/biggieShots/biggieshotS1.wav");
    biggiesShots[1] = Mix_LoadWAV("assets/sound/biggieShots/biggieshotS2.wav");
    biggiesShots[2] = Mix_LoadWAV("assets/sound/biggieShots/biggieshotS3.wav");
    biggiesShots[3] = Mix_LoadWAV("assets/sound/biggieShots/biggieshotS4.wav");
    for (int i = 0; i < 4; i++){
        aSound->biggiesShots[i] = biggiesShots[i];
    }

    Mix_Chunk *lobbySteps = Mix_LoadWAV("assets/sound/lobbyFootsteps.wav");
    aSound->lobbySteps = lobbySteps;
    Mix_Chunk *nemurSteps = Mix_LoadWAV("assets/sound/nemurFootsteps.wav");
    aSound->nemurSteps = nemurSteps;

    Mix_Chunk *playerHit = Mix_LoadWAV("assets/sound/player-hurt.wav");
    aSound->playerHit = playerHit;

    Mix_Chunk *vine = Mix_LoadWAV("assets/sound/Vine Boom.wav");
    aSound->vine = vine;

    Mix_Chunk *enemyFX[5] = {NULL};
    enemyFX[0] = Mix_LoadWAV("assets/sound/elias-is-hurt1.wav");
    enemyFX[1] = Mix_LoadWAV("assets/sound/elias-is-hurt2.wav");
    enemyFX[2] = Mix_LoadWAV("assets/sound/elias-is-hurt3.wav");
    enemyFX[3] = Mix_LoadWAV("assets/sound/elias-is-hurt4.wav");
    enemyFX[4] = Mix_LoadWAV("assets/sound/elias-is-hurt5.wav");
    for (int i = 0; i < 5; i++){
        aSound->enemyFX[i] = enemyFX[i];
    }

    // OBJECTIVE CALLS
    Mix_Chunk *objectiveCall1 = Mix_LoadWAV("assets/sound/linus-mission-call1.2.wav");
    aSound->objectiveCall1 = objectiveCall1;

    Mix_Chunk *objectiveCall2 = Mix_LoadWAV("assets/sound/linus-mission-call2.2.wav");
    aSound->objectiveCall2 = objectiveCall2;

    // MENU SOUND FX
    Mix_Chunk *confirmSound = Mix_LoadWAV("assets/sound/confirmSound.wav");
    aSound->confirmSound = confirmSound;
    Mix_Chunk *hoverSound = Mix_LoadWAV("assets/sound/hoverSound.wav");
    aSound->hoverSound = hoverSound;

    return aSound;
}

void SOUND_destroy(Sound aSound) {
    if (!aSound) return;

    Mix_FreeMusic(aSound->musicMENU);
    Mix_FreeMusic(aSound->musicNEMUR);
    Mix_FreeChunk(aSound->blueShot);
    Mix_FreeChunk(aSound->cleoShot);
    Mix_FreeChunk(aSound->biggieShotLoop);
    for (int i = 0; i < 4; i++){
        Mix_FreeChunk(aSound->biggiesShots[i]);
    }

    Mix_FreeChunk(aSound->dashSound);

    Mix_FreeChunk(aSound->vine);

    Mix_FreeChunk(aSound->objectiveCall1);
    Mix_FreeChunk(aSound->objectiveCall2);
    
    Mix_FreeChunk(aSound->confirmSound);
    Mix_FreeChunk(aSound->hoverSound);

    Mix_FreeChunk(aSound->lobbySteps);
    Mix_FreeChunk(aSound->nemurSteps);

    Mix_FreeChunk(aSound->playerHit);

    for (int i = 0; i < 5; i++){
        Mix_FreeChunk(aSound->enemyFX[i]);
    }
    free(aSound);
}


void SOUND_setVolume(int music, int soundFX){
    float m = roundf(music * 1.28f);
    float s = roundf(soundFX   * 1.28f);

    Mix_VolumeMusic(m);
    Mix_Volume(-1, s);
}

static Mix_Music *newMusic(Sound aSound, MusicTrack newTrack){
    switch (newTrack)
    {
    case MUSIC_MENU:
        return aSound->musicMENU;
        break;
    case MUSIC_LOBBY:
        return aSound->musicMENU;
        break;
    case MUSIC_NEMUR:
        return aSound->musicNEMUR;
        break;
    default:
        break;
    }
    return NULL;
}

void SOUND_setMixVolume(Sound aSound){
    
    Mix_VolumeChunk(aSound->blueShot, 64);
    Mix_VolumeChunk(aSound->biggieShotLoop, 20);
    for (int i = 0; i < 4; i++){
        Mix_VolumeChunk(aSound->biggiesShots[i], 48);
    }

    for (int i = 0; i < 5; i++){
        Mix_VolumeChunk(aSound->enemyFX[i], 48);
    }

    Mix_VolumeChunk(aSound->vine, 128);

    Mix_VolumeChunk(aSound->dashSound, 32);

    Mix_VolumeChunk(aSound->objectiveCall1, 48);
    Mix_VolumeChunk(aSound->objectiveCall2, 48);
    
    Mix_VolumeChunk(aSound->cleoShot, 128);
    Mix_VolumeChunk(aSound->confirmSound, 64);
    Mix_VolumeChunk(aSound->hoverSound, 64);
    Mix_VolumeChunk(aSound->lobbySteps, 32);
    Mix_VolumeChunk(aSound->nemurSteps, 32);
    Mix_VolumeChunk(aSound->playerHit, 72);
}

void SOUND_playMusicIfChanged(Sound aSound, MusicTrack newTrack){
    if (aSound->currentTrack != newTrack) {
        Mix_HaltMusic();
        Mix_PlayMusic(newMusic(aSound, newTrack), -1);
        aSound->currentTrack = newTrack;
    }
}

void SOUND_projectileSoundOnce(Sound aSound, int projectileType, int projIndex, bool isActive) {
    static bool played[MAX_CLIENT_PROJ] = { false };
    static int index = 0;

    if (!isActive) {
        played[projIndex] = false;  // Reset flag when projectile slot is empty
        return;
    }
    if (!played[projIndex]) {
        Mix_Chunk *fx = NULL;

        switch (projectileType) {
            case 1:
                fx = aSound->biggiesShots[index % 4];
                index++;
                break;
            case 3:
                fx = aSound->blueShot;
                break;
            case 4:
                fx = aSound->cleoShot;
                break;
            default:
                return;
        }

        if (fx) {
            Mix_PlayChannel(-1, fx, 0);
            played[projIndex] = true;
        }
    }
}

void SOUND_playDash(Sound aSound) {
    static Uint32 lastPlayedTime = 0;
    Uint32 now = SDL_GetTicks();

    if (aSound->dashSound && (now - lastPlayedTime >= 4000)) {
        Mix_PlayChannel(-1, aSound->dashSound, 0);
        lastPlayedTime = now;
    }
}


void SOUND_playLoopIfRunning(Sound aSound, int playerIndex, bool isRunning, int state) {
    static int channelArray[MAX_CLIENTS] = {-1};
    static bool playingArray[MAX_CLIENTS] = {false};

    Mix_Chunk *steps = NULL;
    if(state == LOBBY){
        steps = aSound->lobbySteps;
    }
    else if(state == NEMUR){
        steps = aSound->nemurSteps;
    }
    
    if (isRunning) {
        if (!playingArray[playerIndex]) {
            channelArray[playerIndex] = Mix_PlayChannel(-1, steps, -1);
            playingArray[playerIndex] = true;
        }
    } else {
        if (playingArray[playerIndex] && channelArray[playerIndex] != -1) {
            Mix_HaltChannel(channelArray[playerIndex]);
            channelArray[playerIndex] = -1;
            playingArray[playerIndex] = false;
        }
    }
}

void SOUND_UIhoverSound(Sound aSound, bool isHover){
    static bool wasHovered = false;

    if (isHover && !wasHovered){
        Mix_PlayChannel(-1, aSound->hoverSound, 0);
    }
    wasHovered = isHover;
}

void SOUND_UIclickSound(Sound aSound) {
    Mix_PlayChannel(-1, aSound->confirmSound, 0);
}

void SOUND_playerIsHurt(Sound aSound) {
    Mix_PlayChannel(-1, aSound->playerHit, 0);
}

void SOUND_enemyIsHurt(Sound aSound) {
    static Uint32 lastTime = 0;
    Uint32 delay = 500;

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastTime >= delay) {
        int randomNumber = rand() % 5;
        Mix_PlayChannel(-1, aSound->enemyFX[randomNumber], 0);
        lastTime = SDL_GetTicks();
    }
}

void SOUND_objectiveSoundCall(Sound aSound, int objective){
    switch (objective)
    {
    case 0:
        Mix_PlayChannel(-1, aSound->objectiveCall1, 0);
        break;
    case 1:
        Mix_PlayChannel(-1, aSound->objectiveCall2, 0);
        break;
    case 2:
        Mix_PlayChannel(-1, aSound->objectiveCall2, 0);
        break;
    
    default:
        break;
    }

}