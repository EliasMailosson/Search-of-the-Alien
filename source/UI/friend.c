#include "../../include/UI/friend.h"
#include "../../include/NET/client.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[MAX_USERNAME_LEN];
    bool isOnline;
} Friend;

typedef struct FriendList {
    Friend friends[MAX_FRIENDS];
    int selectedFriendIndex;
    int count;
    TTF_Font *fonts;
} *FriendList;

FriendList UI_friendListCreate(TTF_Font* font) {
    FriendList list = malloc(sizeof(struct FriendList));
    list->fonts = font;
    list->count = 0;
    list->selectedFriendIndex=-1;
    return list;
}

void UI_DrawFriendList(SDL_Renderer *pRend, FriendList list) {

    UI_friendListRender(list, pRend);
    SDL_Color textColor = {255, 255, 255, 255}; 
    const char *text = "Friends:";

    SDL_Surface *textSurface = TTF_RenderUTF8_Blended(list->fonts, text, textColor);

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(pRend, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect destRect = { 50, 50, textSurface->w, textSurface->h };
    SDL_RenderCopy(pRend, textTexture, NULL, &destRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void UI_FriendremoveSpacesInBetween(char* str){
    char* read = str;
    char* write = str;

    while (*read != '\0') {
        if (!isspace((unsigned char)*read)) {
            *write++ = *read;
        }
        read++;
    }
    *write = '\0';
}

void UI_friendListDestroy(FriendList list) {
    free(list);
}

void UI_SetFriendsOffline(FriendList list) {
    for (int i = 0; i < list->count; i++)
    {
        list->friends[i].isOnline=false;
    }
}
// 2. ser till ifall matchande vänner från clienten är online (CHECKED)
void UI_friendListSetStatus(FriendList list, const char* name) { 
    for (int i = 0; i < list->count; ++i) {
        if (strcmp(list->friends[i].name, name) == 0) {
            list->friends[i].isOnline=true;
            return;
        }
    }
}

void UI_FriendNameToggle(FriendList list, int mouseX, int mouseY) {
    int y = 150;
    list->selectedFriendIndex = -1;

    for (int i = 0; i < list->count; ++i) {
        SDL_Rect friendRect = { 50, y, 200, 40 };
        if (mouseX >= friendRect.x && mouseX <= friendRect.x + friendRect.w &&
            mouseY >= friendRect.y && mouseY <= friendRect.y + friendRect.h) {
            list->selectedFriendIndex=-1;
            printf("Vald vän: %s\n", list->friends[i].name);
            break;
        }
        y += 60;
    }
}
//"Sardor"
void UI_friendListRender(FriendList list, SDL_Renderer* renderer) {
    int y = 150;
    for (int i = 0; i < list->count; ++i) {
        SDL_Color color = list->friends[i].isOnline ? 
            (SDL_Color){0, 255, 0, 255} : 
            (SDL_Color){255, 0, 0, 255};

        char statusText[64];
        snprintf(statusText, sizeof(statusText), "%s", list->friends[i].name);

        SDL_Surface* surface = TTF_RenderText_Solid(list->fonts, statusText, color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect dest = { 50, y, surface->w, surface->h };

        if (i==list->selectedFriendIndex)
        {
            SDL_Rect highlight = {45, y - 5, 220, surface->h + 10};
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
            SDL_RenderFillRect(renderer, &highlight);
        }

        SDL_RenderCopy(renderer, texture, NULL, &dest);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        y += 60;
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
        fprintf(fp,"%s", aFriendList->friends[i].name);
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
    }
    fclose(fp);
}