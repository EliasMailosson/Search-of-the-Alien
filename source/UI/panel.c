#include "../../include/UI/panel.h"
#include <stdlib.h>
#include <string.h>
#include "../../include/menu.h"
#include "../../include/UI/label.h"
#include "../../include/UI/button.h"
#include "../../include/UI/checklist.h"
#include "../../include/UI/inputfield.h"
#include "../../include/UI/friend.h"
#include "../../include/UI/animation.h"

typedef struct component {
    void* pComp;
    char key[64];
    int type;
    bool hasPanelLink;
    int panelLink;
} Component;

typedef struct Panel {
    SDL_Rect rect;
    SDL_Color bg;
    SDL_Texture* pBgImage;
    bool hasImage;
    bool active;

    Component compList[20];
    int compCount;
} *Panel;

Panel UI_panelCreate() {
    Panel aPanel = malloc(sizeof(struct Panel));

    aPanel->compCount = 0;
    aPanel->bg = (SDL_Color){ .r = 0, .g = 0, .b = 0, .a = 0 };
    aPanel->rect = (SDL_Rect){ .x = 0, .y = 0, .w = 1, .h = 1 };
    aPanel->pBgImage = NULL;
    aPanel->hasImage = false;
    aPanel->active = true;

    return aPanel;
}

void UI_panelSetImage(SDL_Renderer* pRend, Panel aPanel, char *imagePath) {
    if(aPanel->pBgImage) SDL_DestroyTexture(aPanel->pBgImage);
    SDL_Surface *surface = IMG_Load(imagePath);
    aPanel->pBgImage = SDL_CreateTextureFromSurface(pRend, surface);
    SDL_FreeSurface(surface);
    aPanel->hasImage = true;
}

void UI_panelSetActive(Panel aPanel, bool active) {
    aPanel->active = active;
}

void UI_panelAddComponent(Panel aPanel, void* comp, int type, char* key) {
    if (aPanel->compCount < 20) {
        aPanel->compList[aPanel->compCount].pComp = comp;
        aPanel->compList[aPanel->compCount].type = type;
        strcpy(aPanel->compList[aPanel->compCount].key, key);
        aPanel->compList[aPanel->compCount].hasPanelLink = false;
        (aPanel->compCount)++;
    }
}

void UI_panelSetComponentLink(Panel aPanel, char* key, int panelLink) {
    for(int i = 0; i < aPanel->compCount; i++) {
        if(strcmp(key, aPanel->compList[i].key) == 0 && aPanel->compList[i].type == UI_BUTTON) {
            aPanel->compList[i].panelLink = panelLink;
            aPanel->compList[i].hasPanelLink = true;
            break;
        }
    }
}

void* UI_panelGetComponent(Panel aPanel, char *key) {
    for(int i = 0; i < aPanel->compCount; i++) {
        if(strcmp(key, aPanel->compList[i].key) == 0) {
            return aPanel->compList[i].pComp;
        }
    }
    return NULL;
}

void UI_panelSetAppearance(Panel aPanel, SDL_Rect rect, SDL_Color src_bg) {
    aPanel->bg = src_bg;
    aPanel->rect = rect;
}

void UI_panelUpdate(Panel aPanel, MenuEvent *pEvent, bool isMouseUp, bool *keys, Sound aSound) {
    if(aPanel == NULL) return;
    if(!aPanel->active) return;
    bool nowHover = false;

    pEvent->eventType = PANEL_IDLE;

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    for (int i = 0; i < aPanel->compCount; i++) {
        switch (aPanel->compList[i].type) {
        case UI_LABEL: break;

        case UI_BUTTON:
            if(UI_buttonIsHovered((Button)aPanel->compList[i].pComp, mouseX, mouseY)) {
                nowHover = true;
                if(isMouseUp) {
                    SOUND_UIclickSound(aSound);

                    // pEvent->eventType = BUTTON_CLICKED;
                    // printf("Button Clicked! (key: %s)\n", aPanel->compList[i].key);
                    if(aPanel->compList[i].hasPanelLink) {
                        
                        pEvent->eventType = PANEL_SWITCH;
                        pEvent->newPanel = aPanel->compList[i].panelLink;
                        strcpy(pEvent->key, aPanel->compList[i].key);
                    } 
                    else {
                        pEvent->eventType = BUTTON_CLICKED;
                        strcpy(pEvent->key, aPanel->compList[i].key);
                    }
                    return;
                }
            }
            
            break;
        
        case UI_CHECKLIST:
            if(isMouseUp) {
                pEvent->eventType = CHECKLIST_UPDATED;
                bool values[20];
                UI_checklistToggle((Checklist)aPanel->compList[i].pComp, mouseX, mouseY);
                UI_ChecklistGetItemValues((Checklist)aPanel->compList[i].pComp, values);
                // for(int i = 0; i < 20; i++) {
                //     printf("checkbox %d: (%s)\n", i, (values[i]) ? "true" : "false");
                // }
                return;
            }
            break;
        
        case UI_INPUTFIELD:
            if(isMouseUp) {
                UI_inputfieldSetFocus((Inputfield)aPanel->compList[i].pComp, mouseX, mouseY);
            }
            if(UI_inputfieldIsFocused((Inputfield)aPanel->compList[i].pComp)) {
                if(keys[SDL_SCANCODE_RIGHT]) {
                    UI_inputfieldMoveCursor((Inputfield)aPanel->compList[i].pComp, true);
                } else if(keys[SDL_SCANCODE_LEFT]) {
                    UI_inputfieldMoveCursor((Inputfield)aPanel->compList[i].pComp, false);
                } else if(keys[SDL_SCANCODE_BACKSPACE]) {
                    UI_inputfieldBackspace((Inputfield)aPanel->compList[i].pComp);
                } else if(pEvent->isTextInput) {
                    UI_inputfieldUpdateBuffer((Inputfield)aPanel->compList[i].pComp, pEvent->textInput);
                }
            }
            
            break;
        
        case UI_FRIENDLIST:
                if (isMouseUp) {
                    //här ska UI_FriendNameToggle(); va
                }
                
            break;

        case UI_ANIMATION: {
                int mode = UI_animationGetMode((Animation)aPanel->compList[i].pComp);
                if(mode == PLAYBACK_FORWARD) {
                    UI_animationNextFrame((Animation)aPanel->compList[i].pComp);
                } else if(mode == PLAYBACK_BACKWARD) {
                    UI_animationPrevFrame((Animation)aPanel->compList[i].pComp);
                }
                if(UI_animationGetColumn((Animation)aPanel->compList[i].pComp) == 0) {
                    UI_animationSetMode((Animation)aPanel->compList[i].pComp, PLAYBACK_STOP);
                }
                break;
            }
        }
    }
    SOUND_UIhoverSound(aSound, nowHover);
}

void UI_panelRender(SDL_Renderer* pRend, Panel aPanel) {
    if(!aPanel->active) return;

    if (aPanel->hasImage) {
        SDL_RenderCopy(pRend, aPanel->pBgImage, NULL, &aPanel->rect);
    }
    else {
        SDL_SetRenderDrawColor(pRend, aPanel->bg.r, aPanel->bg.g, aPanel->bg.b, aPanel->bg.a);
        SDL_RenderFillRect(pRend, &aPanel->rect);
    }

    for (int i = 0; i < aPanel->compCount; i++) {
        switch (aPanel->compList[i].type) {
        case UI_LABEL:
            UI_labelRender(pRend, (Label)aPanel->compList[i].pComp);
            break;
        case UI_BUTTON:
            UI_buttonRenderer(pRend, (Button)aPanel->compList[i].pComp);
            break;
        
        case UI_CHECKLIST:
            UI_checklistRendrer(pRend, (Checklist)aPanel->compList[i].pComp);
            break;

        case UI_INPUTFIELD:
            UI_inputfieldRefreshTexture(pRend, (Inputfield)aPanel->compList[i].pComp);
            UI_inputfieldRender(pRend, (Inputfield)aPanel->compList[i].pComp);
            break;

        case UI_FRIENDLIST:
            UI_DrawFriendList(pRend,(FriendList)aPanel->compList[i].pComp); 
            break;

        case UI_ANIMATION:
            UI_animationRender(pRend,(Animation)aPanel->compList[i].pComp);
            break;

        }
    }
}

void UI_panelDestroy(Panel aPanel) {
    for (int i = 0; i < aPanel->compCount; i++) {
        switch (aPanel->compList[i].type) {
        case UI_LABEL:
            UI_labelDestroy((Label)aPanel->compList[i].pComp);
            break;

        case UI_BUTTON:
            UI_buttonDestroy((Button)aPanel->compList[i].pComp);
            break;
        
        case UI_CHECKLIST:
            UI_ChecklistDestroy((Checklist)aPanel->compList[i].pComp);
            break;

        case UI_INPUTFIELD:
            UI_inputfieldDestroy((Inputfield)aPanel->compList[i].pComp);
            break;

        case UI_FRIENDLIST:
            UI_friendListDestroy((FriendList)aPanel->compList[i].pComp);
            break;
        
        case UI_ANIMATION:
            UI_animationDestroy((Animation)aPanel->compList[i].pComp);
            break;

        }
        aPanel->compList[i].pComp = NULL;
    }
    SDL_DestroyTexture(aPanel->pBgImage);
    aPanel->pBgImage = NULL;
    free(aPanel);
    aPanel = NULL;
}