#include "../../include/UI/label.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Item { 
    bool is_Checked;
} Item;

typedef struct Checklist {
    int x, y;
    int checklist_count;
    SDL_Rect Rect;
    Item items[20];
} *Checklist;



Checklist UI_checklistCreate(){
    Checklist aChecklist = malloc(sizeof(struct Checklist));
    if(aChecklist == NULL) {
        return NULL;
    }
    

    aChecklist->x = 400;
    aChecklist->y = 300;
    aChecklist->checklist_count = 3;
    aChecklist->Rect.x = 200;
    aChecklist->Rect.y = 150;
    aChecklist->Rect.w = 30;
    aChecklist->Rect.h = 30;

    for (int i = 0; i < aChecklist->checklist_count; i++)
    {
        aChecklist->items[i].is_Checked = true;
    }

    return aChecklist;
}


void UI_checklistRendrer(SDL_Renderer* pRend, Checklist achecklist){
    for (int i = 0; i < achecklist->checklist_count; i++){
        SDL_Rect checkboxRect = achecklist->Rect;
        checkboxRect.y += i* (checkboxRect.h + 10); //mellanrum mellan checkboxerna

        SDL_SetRenderDrawColor(pRend, 255,255,255,255); //vit
        SDL_RenderDrawRect(pRend,&checkboxRect);

        // if (achecklist->items[i].is_Checked) {
        //     SDL_SetRenderDrawColor(pRend, 0, 255, 0, 255);  //färg på själva checkmarken
        //     SDL_RenderFillRect(pRend, &checkboxRect);
        // }

        if (achecklist->items[i].is_Checked){
            SDL_SetRenderDrawColor(pRend,0 ,0 ,0 ,255); //svart 

            int startX = checkboxRect.x + 6;
            int startY =  checkboxRect.y + checkboxRect.h / 2;

            int midX = checkboxRect.x  + checkboxRect.w / 3;
            int midY = checkboxRect.y + checkboxRect.h - 6;

            int endX = checkboxRect.x + checkboxRect.w - 6;
            int endY = checkboxRect.y + 6;

            SDL_RenderDrawLine(pRend, startX, startY, midX, midY); // "\"
            SDL_RenderDrawLine(pRend, midX, midY, endX, endY); // "/"
        }   
    }
}

void UI_ChecklistAddItem(Checklist checklist) {
    if(checklist == NULL) return;
    if(checklist->checklist_count < 20) {
        checklist->items[checklist->checklist_count].is_Checked = false;
        (checklist->checklist_count)++;
    }
}

void UI_ChecklistGetItemValues(Checklist checklist, bool *value) {
    for (int i = 0; i < 20; i++) {
        value[i] = checklist->items[i].is_Checked;
    }
}

void UI_ChecklistDestroy(Checklist checklist) {
    if (checklist == NULL) {
        return;
    }
    free(checklist);
}

void UI_checklistToggle(Checklist achecklist, int mouseX, int mouseY){

    for (int i = 0; i < achecklist->checklist_count; i++){
        SDL_Rect checkboxRect = achecklist->Rect;
        checkboxRect.y += i * (checkboxRect.h + 10); //mellanrum mellan checkboxerna
        
        if (mouseX >= checkboxRect.x && mouseX <= checkboxRect.x + checkboxRect.w && mouseY >= checkboxRect.y && mouseY <= checkboxRect.y + checkboxRect.h)
        {
            achecklist->items[i].is_Checked = !achecklist->items[i].is_Checked;
            break;
        }
    }
}
