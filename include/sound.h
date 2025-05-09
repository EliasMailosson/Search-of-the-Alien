#ifndef SOUND_H
#define SOUND_H

#define MAX_SOUND_CHANNELS 16

typedef enum {
    MUSIC_NONE,
    MUSIC_MENU,
    MUSIC_LOBBY,
    MUSIC_NEMUR,
} MusicTrack;

typedef struct sound{
    Mix_Music *musicMENU;
    Mix_Music *musicNEMUR;
    MusicTrack currentTrack;

    Mix_Chunk *blueShot;
    Mix_Chunk *biggieShot[16];
    Mix_Chunk *cleoShot;


    Mix_Chunk *lobbySteps;
    Mix_Chunk *nemurSteps;

    Mix_Chunk *confirmSound;
    Mix_Chunk *hoverSound;

    int currentChannel[MAX_SOUND_CHANNELS];
    int masterVolume;
}sound;

typedef struct sound *Sound;

Sound SOUND_create(void);

void SOUND_destroy(Sound aSound);

void SOUND_setVolume(int music, int soundFX);

void SOUND_playMusicIfChanged(Sound aSound , MusicTrack newTrack);

bool SOUND_timedSoundFX(Uint32 *lastPlayedTime, Uint32 timeMs);

bool SOUND_isChunkPlaying(Sound aSound, Mix_Chunk *chunk);

// void SOUND_projectileSound(int *channel, Mix_Chunk *soundFX, int currentProjectileCount, int lastProjectileCount);

void SOUND_projectileSoundOnce(Sound aSound, int projectileType, int projIndex, bool isActive);

void SOUND_playLoopIfRunning(Sound aSound, int playerIndex, bool isRunning, int *channelArray, bool *playingArray, GameState state);


#endif