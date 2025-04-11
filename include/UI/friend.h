#ifndef FRIEND_H
#define FRIEND_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>

typedef struct FriendList* FriendList;

FriendList UI_friendListCreate();
void UI_friendListDestroy(FriendList list);

bool UI_friendListAdd(FriendList list, const char* name, bool isOnline);
void UI_friendListSetStatus(FriendList list, const char* name, bool isOnline);
void UI_friendListRender(FriendList list, SDL_Renderer* renderer, TTF_Font* font);
void UI_DrawFriendList(SDL_Renderer *pRend, TTF_Font *font);


#endif
