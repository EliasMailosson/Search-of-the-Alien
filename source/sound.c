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
    Mix_Chunk *blueShot = Mix_LoadWAV("assets/sound/blueShot.mp3");
    aSound->blueShot = blueShot;
    //Mix_Chunk *biggieShot = Mix_LoadWAV("assets/sound/biggieShotShort.wav");
    //aSound->biggieShot = biggieShot;
    Mix_Chunk *cleoShot = Mix_LoadWAV("assets/sound/cleoShot.wav");
    aSound->cleoShot = cleoShot;

    Mix_Chunk *biggieShot[16];
    for (int i = 0; i < 16; i++) {
        char filename[64];
        sprintf(filename, "assets/sound/biggieShots/biggieShot%d.wav", i + 1);  // 1 to 16

        biggieShot[i] = Mix_LoadWAV(filename);
        if (!biggieShot[i]) {
            fprintf(stderr, "Failed to load %s: %s\n", filename, Mix_GetError());
        }
        aSound->biggieShot[i] = biggieShot[i];
    }

    Mix_Chunk *lobbySteps = Mix_LoadWAV("assets/sound/lobbyFootsteps.wav");
    aSound->lobbySteps = lobbySteps;
    Mix_Chunk *nemurSteps = Mix_LoadWAV("assets/sound/nemurFootsteps.wav");
    aSound->nemurSteps = nemurSteps;

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
    
    for (int i = 0; i < 16; i++)
    {
        Mix_FreeChunk(aSound->biggieShot[i]);
    }
    
    Mix_FreeChunk(aSound->cleoShot);
    Mix_FreeChunk(aSound->confirmSound);
    Mix_FreeChunk(aSound->hoverSound);
    Mix_FreeChunk(aSound->lobbySteps);
    Mix_FreeChunk(aSound->nemurSteps);

    free(aSound);
}

void SOUND_setVolume(int music, int soundFX){
    Mix_VolumeMusic(music);
    Mix_Volume(-1, soundFX);
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

void SOUND_projectileSoundOnce(Sound aSound, int projectileType, int projIndex, bool isActive) {
    static bool played[MAX_CLIENT_PROJ] = { false };
    static int biggieShotIndex = 0;  // Shared index for cycling

    if (!isActive) {
        played[projIndex] = false;  // Reset flag when projectile slot is empty
        return;
    }

    if (!played[projIndex]) {
        Mix_Chunk *fx = NULL;

        switch (projectileType) {
            case 1:
                fx = aSound->biggieShot[biggieShotIndex];
                biggieShotIndex = (biggieShotIndex + 1) % 16;  // Cycle 0–15
                printf("%d\n", biggieShotIndex);
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


void SOUND_playLoopIfRunning(Sound aSound, int playerIndex, bool isRunning, int *channelArray, bool *playingArray, GameState state) {
    Mix_Chunk *steps = NULL;
    if(state == LOBBY){
        steps = aSound->lobbySteps;
    }
    else if(state == NEMUR){
        steps = aSound->nemurSteps;
    }
    
    if (isRunning) {
        if (!playingArray[playerIndex]) {
            Mix_VolumeChunk(steps, 64);
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