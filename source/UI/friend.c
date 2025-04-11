#include "../../include/UI/friend.h"
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[MAX_USERNAME_LEN];
    bool isOnline;
} Friend;

typedef struct FriendList {
    Friend friends[MAX_FRIENDS];
    int count;
} *FriendList;

FriendList UI_friendListCreate() {
    FriendList list = malloc(sizeof(struct FriendList));
    if (!list) {
        SDL_Log("Misslyckades att allokera minne för vänlista");
        return NULL;
    }
    list->count = 0;
    return list;
}

void UI_DrawFriendList(SDL_Renderer *pRend, TTF_Font *font) {
    if (!font) {
        SDL_Log("Font saknas i UI_DrawFriendList");
        return;
    }

    SDL_Color textColor = {255, 255, 255, 255}; // Vit text
    const char *text = "Friends:";

    SDL_Surface *textSurface = TTF_RenderUTF8_Blended(font, text, textColor);
    if (!textSurface) {
        SDL_Log("Kunde inte rendera text: %s", TTF_GetError());
        return;
    }

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(pRend, textSurface);
    if (!textTexture) {
        SDL_Log("Kunde inte skapa texture: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect destRect = { 50, 50, textSurface->w, textSurface->h };
    SDL_RenderCopy(pRend, textTexture, NULL, &destRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void UI_friendListDestroy(FriendList list) {
    free(list);
}

bool UI_friendListAdd(FriendList list, const char* name, bool isOnline) {
    if (list->count >= MAX_FRIENDS) return false;

    strncpy(list->friends[list->count].name, name, MAX_USERNAME_LEN - 1);
    list->friends[list->count].name[MAX_USERNAME_LEN - 1] = '\0';
    list->friends[list->count].isOnline = isOnline;

    list->count++;
    return true;
}

void UI_friendListSetStatus(FriendList list, const char* name, bool isOnline) {
    for (int i = 0; i < list->count; ++i) {
        if (strcmp(list->friends[i].name, name) == 0) {
            list->friends[i].isOnline = isOnline;
            return;
        }
    }
}

void UI_friendListRender(FriendList list, SDL_Renderer* renderer, TTF_Font* font) {
    int y = 150;
    for (int i = 0; i < list->count; ++i) {
        SDL_Color color = list->friends[i].isOnline ? 
            (SDL_Color){0, 255, 0, 255} : 
            (SDL_Color){255, 0, 0, 255};

        char statusText[64];
        snprintf(statusText, sizeof(statusText), "%s (%s)",
                 list->friends[i].name,
                 list->friends[i].isOnline ? "Online" : "Offline");

        SDL_Surface* surface = TTF_RenderText_Solid(font, statusText, color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect dest = { 100, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &dest);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        y += 40;
    }
}

void UI_clientAddFriend(FriendList aFriendList, char *outputFriend){
    if (aFriendList->count >= MAX_FRIENDS) return; // Prevent overflow

    strncpy(aFriendList->friends[aFriendList->count].name, outputFriend, MAX_USERNAME_LEN - 1);
    aFriendList->friends[aFriendList->count].name[MAX_USERNAME_LEN - 1] = '\0';
    aFriendList->count++;
}

void UI_updateFriendList(FriendList aFriendList){
    FILE *fp;
    fp = fopen("data/playerlist.txt", "w");
    if (fp != NULL)
    {
      fprintf(fp,"%d\n", aFriendList->count);
      for (int i = 0; i < aFriendList->count; i++)
      {
        fprintf(fp,"%s ", aFriendList->friends[i].name);
        fprintf(fp,"\n");
      }
    }
    fclose(fp);
}

void UI_readFriendList(FriendList aFriendList){
    FILE *fp;
    fp = fopen("data/playerlist.txt", "r");
    if(fp != NULL)
    {
        char username[MAX_USERNAME_LEN];
        fscanf(fp,"%d", &aFriendList->count);
        for (int i = 0; i < aFriendList->count; i++)
        {
            fscanf(fp,"%s", username);
            strncpy(aFriendList->friends[i].name, username, MAX_USERNAME_LEN - 1);
            aFriendList->friends[i].name[MAX_USERNAME_LEN - 1] = '\0';
        }
        printf("%d\n", aFriendList->count);
        for (int i = 0; i < aFriendList->count; i++)
        {
            printf("%s\n", aFriendList->friends[i].name);
        }
    }
    fclose(fp);
}