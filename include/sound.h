#ifndef SOUND_H
#define SOUND_H

typedef enum {
    MUSIC_NONE,
    MUSIC_MENU,
    MUSIC_LOBBY,
    MUSIC_NEMUR,
} MusicTrack;

typedef struct sound Sound;

void playMusicIfChanged(Mix_Music *newMusic, MusicTrack newTrack, MusicTrack *currentTrack);

#endif