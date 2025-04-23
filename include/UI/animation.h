#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>

enum PlaybackModes{PLAYBACK_STOP, PLAYBACK_FORWARD, PLAYBACK_BACKWARD};

typedef struct Animation* Animation;

Animation UI_animationCreate();
void UI_animationDestroy(Animation aAnimation);
void UI_animationRender(SDL_Renderer *pRend, Animation aAnimation);

/*Go forward 1 frame*/
void UI_animationNextFrame(Animation aAnimation);
/*Go backward 1 frame*/
void UI_animationPrevFrame(Animation aAnimation);

void UI_animationLoad(Animation aAnimation, SDL_Renderer *pRend, char *imgPath, int frameW, int frameH, int frameCount);
void UI_animationSetDestination(Animation aAnimation, SDL_Rect rect);

int UI_animationGetMode(Animation aAnimation);
void UI_animationSetMode(Animation aAnimation, int newMode);

#endif