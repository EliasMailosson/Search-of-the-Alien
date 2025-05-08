#include "../include/clientLife.h"
#include "../include/sound.h"

struct sound{
    Mix_Music *backgroundMusic1;
    Mix_Music *backgroundMusicNEMUR;
    MusicTrack currentMusic;

    
};

void playMusicIfChanged(Mix_Music *newMusic, MusicTrack newTrack, MusicTrack *currentTrack){
    if (*currentTrack != newTrack) {
        Mix_HaltMusic();
        Mix_PlayMusic(newMusic, -1);
        *currentTrack = newTrack;
    }
}

bool canPlaySoundFX(Uint32 *lastPlayedTime, Uint32 timeMs){
    Uint32 now = SDL_GetTicks();
    if (now - *lastPlayedTime >= timeMs) {
        *lastPlayedTime = now;
        return true;
    }
    return false;
}

bool isChunkPlaying(Mix_Chunk *chunk) {
    int totalChannels = Mix_AllocateChannels(-1);  // get current number of channels

    for (int ch = 0; ch < totalChannels; ++ch) {
        if (Mix_GetChunk(ch) == chunk && Mix_Playing(ch)) {
            return 1;  // Yes, this chunk is playing on channel ch
        }
    }
    return 0;
}