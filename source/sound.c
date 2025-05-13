#include "../include/clientLife.h"
#include "../include/sound.h"

Sound SOUND_create(void) {
    Sound aSound = malloc(sizeof(*aSound));
    if (!aSound) {
        fprintf(stderr, "Error allocating Sound struct\n");
        return NULL;
    }
    aSound->currentTrack = MUSIC_NONE;
    aSound->masterVolume = 64;  // 50%
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
    //Mix_Chunk *biggieShot = Mix_LoadWAV("assets/sound/biggieShotShort.wav");
    //aSound->biggieShot = biggieShot;
    Mix_Chunk *cleoShot = Mix_LoadWAV("assets/sound/cleoShot.wav");
    aSound->cleoShot = cleoShot;

    Mix_Chunk * biggieShotLoop = Mix_LoadWAV("assets/sound/biggieShot2step.wav");
    aSound->biggieShotLoop = biggieShotLoop;

    /* Mix_Chunk *biggieShot[16];
    for (int i = 0; i < 16; i++) {
        char filename[64];
        sprintf(filename, "assets/sound/biggieShots/biggieShot%d.wav", i + 1);  // 1 to 16

        biggieShot[i] = Mix_LoadWAV(filename);
        if (!biggieShot[i]) {
            fprintf(stderr, "Failed to load %s: %s\n", filename, Mix_GetError());
        }
        aSound->biggieShot[i] = biggieShot[i];
    }*/

    Mix_Chunk *lobbySteps = Mix_LoadWAV("assets/sound/lobbyFootsteps.wav");
    aSound->lobbySteps = lobbySteps;
    Mix_Chunk *nemurSteps = Mix_LoadWAV("assets/sound/nemurFootsteps.wav");
    aSound->nemurSteps = nemurSteps;

    Mix_Chunk *playerHit = Mix_LoadWAV("assets/sound/player-hurt3.wav");
    aSound->playerHit = playerHit;

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
    Mix_FreeChunk(aSound->biggieShotLoop);
    
    /*for (int i = 0; i < 16; i++)
    {
        Mix_FreeChunk(aSound->biggieShot[i]);
    }*/

    Mix_FreeChunk(aSound->dashSound);
    
    Mix_FreeChunk(aSound->cleoShot);
    Mix_FreeChunk(aSound->confirmSound);
    Mix_FreeChunk(aSound->hoverSound);
    Mix_FreeChunk(aSound->lobbySteps);
    Mix_FreeChunk(aSound->nemurSteps);
    Mix_FreeChunk(aSound->playerHit);

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
    Mix_VolumeChunk(aSound->biggieShotLoop, 128);
    
    /*for (int i = 0; i < 16; i++)
    {
        Mix_VolumeChunk(aSound->biggieShot[i], 128);
    }
    */
    Mix_VolumeChunk(aSound->dashSound, 32);
    
    Mix_VolumeChunk(aSound->cleoShot, 128);
    Mix_VolumeChunk(aSound->confirmSound, 64);
    Mix_VolumeChunk(aSound->hoverSound, 64);
    Mix_VolumeChunk(aSound->lobbySteps, 24);
    Mix_VolumeChunk(aSound->nemurSteps, 24);
    Mix_VolumeChunk(aSound->playerHit, 72);


}

void SOUND_playMusicIfChanged(Sound aSound, MusicTrack newTrack){
    if (aSound->currentTrack != newTrack) {
        Mix_HaltMusic();
        Mix_PlayMusic(newMusic(aSound, newTrack), -1);
        aSound->currentTrack = newTrack;
    }
}

bool SOUND_timedSoundFX(Uint32 *lastPlayedTime, Uint32 timeMs){
    Uint32 now = SDL_GetTicks();
    if (now - *lastPlayedTime >= timeMs) {
        *lastPlayedTime = now;
        return true;
    }
    return false;
}

bool SOUND_isChunkPlaying(Sound aSound, Mix_Chunk *chunk) {
    for (int ch = 0; ch < MAX_SOUND_CHANNELS; ++ch) {
        if (Mix_GetChunk(ch) == chunk && Mix_Playing(ch)) {
            return 1;  // Yes, this chunk is playing on channel ch
        }
    }
    return 0;
}

/* void SOUND_projectileSound(int *channel, Mix_Chunk *soundFX, int currentProjectileCount, int lastProjectileCount){
    if (currentProjectileCount > lastProjectileCount){
        if (*channel == -1 || !Mix_Playing(*channel)){
            *channel = Mix_PlayChannel(-1, soundFX, currentProjectileCount - 1);  // Loop while projectiles exist
        }
    } 
    else if(currentProjectileCount <= lastProjectileCount){
        if (*channel != -1 && Mix_Playing(*channel)){
            Mix_HaltChannel(*channel);
            *channel = -1;
        }
    }
}*/

/*void SOUND_biggieLoopControl(Sound aSound, Proj *projList, int currentBiggieCount) {
    static bool biggiePlaying = false;
    static int biggieChannel = -1;
    static int lastBiggieCount = 0;


    // Start loop if new biggie projectiles appeared
    if (currentBiggieCount > lastBiggieCount && !biggiePlaying) {
        biggieChannel = Mix_PlayChannel(-1, aSound->biggieShotLoop, -1);
        biggiePlaying = true;
    }

    // Stop loop if all biggie projectiles are gone
    if (currentBiggieCount < lastBiggieCount - 6 && biggiePlaying || currentBiggieCount == 0 && biggiePlaying ) {
        Mix_HaltChannel(biggieChannel);
        biggieChannel = -1;
        biggiePlaying = false;
    }

    lastBiggieCount = currentBiggieCount;
}*/


void SOUND_projectileSoundOnce(Sound aSound, int projectileType, int projIndex, bool isActive) {
    static bool played[MAX_CLIENT_PROJ] = { false };

    if (!isActive) {
        played[projIndex] = false;  // Reset flag when projectile slot is empty
        return;
    }

    if (!played[projIndex]) {
        Mix_Chunk *fx = NULL;

        switch (projectileType) {
            case 1:
                
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

        if (fx && projectileType != 1) {
            Mix_PlayChannel(-1, fx, 0);
            played[projIndex] = true;
        }
    }
}

void SOUND_playDash(Sound aSound) {
    static Uint32 lastPlayedTime = 0;
    Uint32 now = SDL_GetTicks();

    if (aSound->dashSound && (now - lastPlayedTime >= 4000)) {  // 5000 ms = 5 seconds
        //Mix_VolumeChunk(aSound->dashSound, 64);
        Mix_PlayChannel(-1, aSound->dashSound, 0);
        lastPlayedTime = now;
    }
}


void SOUND_playLoopIfRunning(Sound aSound, int playerIndex, bool isRunning, int *channelArray, bool *playingArray, int state) {
    Mix_Chunk *steps = NULL;
    if(state == LOBBY){
        steps = aSound->lobbySteps;
    }
    else if(state == NEMUR){
        steps = aSound->nemurSteps;
    }
    
    if (isRunning) {
        if (!playingArray[playerIndex]) {
            // Mix_VolumeChunk(steps, 32);
            channelArray[playerIndex] = Mix_PlayChannel(-1, steps, -1);  // Loop
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
        //Mix_VolumeChunk(aSound->hoverSound, 64);
        Mix_PlayChannel(-1, aSound->hoverSound, 0);
    }
    wasHovered = isHover;
}

void SOUND_UIclickSound(Sound aSound) {
    //Mix_VolumeChunk(aSound->hoverSound, 1);
    Mix_PlayChannel(-1, aSound->confirmSound, 0);
}

void SOUND_playerIsHurt(Sound aSound) {
    //Mix_VolumeChunk(aSound->hoverSound, 1);
    Mix_PlayChannel(-1, aSound->playerHit, 0);
}