#ifndef SOUND_H
#define SOUND_H

#include <SDL_mixer.h>

#define MAX_SOUND_CHANNELS 32

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

    Mix_Chunk *biggieShotLoop;

    Mix_Chunk *dashSound;

    Mix_Chunk *lobbySteps;
    Mix_Chunk *nemurSteps;

    Mix_Chunk *playerHit;

    Mix_Chunk *confirmSound;
    Mix_Chunk *hoverSound;

    int currentChannel[MAX_SOUND_CHANNELS];
    int masterVolume;
}sound;

typedef struct sound *Sound;

Sound SOUND_create(void);

void SOUND_destroy(Sound aSound);

/**
 * Set Master Volume for Music and Sound Effects
 *
 * This function lets you set your game’s overall
 * music and SFX volumes using a 0–100 range
 * (where 0 = mute, 100 = full volume).
 * 
 * Example:  
 * ```c
 * SOUND_setVolume(75, 50);
 * ```
 * 
 * \param music    Desired music volume (0 – 100)
 * \param soundFX  Desired sound effects volume (0 – 100)
 */
void SOUND_setVolume(int music, int soundFX);

void SOUND_setMixVolume(Sound aSound);

void SOUND_playMusicIfChanged(Sound aSound , MusicTrack newTrack);

bool SOUND_timedSoundFX(Uint32 *lastPlayedTime, Uint32 timeMs);

bool SOUND_isChunkPlaying(Sound aSound, Mix_Chunk *chunk);

// void SOUND_projectileSound(int *channel, Mix_Chunk *soundFX, int currentProjectileCount, int lastProjectileCount);

// void SOUND_biggieLoopControl(Sound aSound, Proj *projList, int currentBiggieCount);

void SOUND_projectileSoundOnce(Sound aSound, int projectileType, int projIndex, bool isActive);

void SOUND_playDash(Sound aSound);

void SOUND_playLoopIfRunning(Sound aSound, int playerIndex, bool isRunning, int state);

void SOUND_UIhoverSound(Sound aSound, bool isHover);

void SOUND_UIclickSound(Sound aSound);

void SOUND_playerIsHurt(Sound aSound);

#endif