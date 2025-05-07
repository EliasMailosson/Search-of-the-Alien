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