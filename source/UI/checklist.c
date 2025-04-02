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
    aChecklist->Rect.w = 20;
    aChecklist->Rect.h = 20;

    for (int i = 0; i < aChecklist->checklist_count; i++)
    {
        aChecklist->items[i].is_Checked = false;
    }

    return aChecklist;
}


void UI_checklistRendrer(SDL_Renderer* pRend, Checklist achecklist){
    for (int i = 0; i < achecklist->checklist_count; i++){
        SDL_Rect checkboxRect = achecklist->Rect;
        checkboxRect.y += i* (checkboxRect.h + 10); //mellanrum mellan checkboxerna

        SDL_SetRenderDrawColor(pRend, 255,255,255,255); //vit
        SDL_RenderDrawRect(pRend,&checkboxRect);

        if (achecklist->items[i].is_Checked) {
            SDL_SetRenderDrawColor(pRend, 0, 255, 0, 255);  //färg på själva checkmarken
            SDL_RenderFillRect(pRend, &checkboxRect);
        }

        if (achecklist->items[i].is_Checked){
            SDL_SetRenderDrawColor(pRend,0 ,0 ,0 ,255); //svart 
            SDL_RenderDrawLine(pRend,checkboxRect.x,checkboxRect.y,checkboxRect.x + checkboxRect.w, checkboxRect.y+checkboxRect.h);// "/"
            SDL_RenderDrawLine(pRend,checkboxRect.x,checkboxRect.y,checkboxRect.x + checkboxRect.w, checkboxRect.y + checkboxRect.h);// "\"
        }   
    }
}

void UI_ChecklistAddItem(Checklist checklist, char *txt) {
    if (checklist->checklist_count<20){
        checklist->items[checklist->checklist_count].is_Checked = false;
        checklist->checklist_count++;
    }
}

void UI_ChecklistDestroy(Checklist checklist) {
    if (checklist == NULL) {
        return;
    }
    free(checklist);
}

void UI_checklistSetPosition(){

}
