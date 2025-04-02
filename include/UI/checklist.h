#ifndef CHECKLIST_H
#define CHECKLIST_H

#include <stdbool.h>
#include <SDL.h>

typedef struct Checklist *Checklist;

Checklist UI_checklistCreate();
void UI_checklistRendrer(SDL_Renderer* pRend, Checklist achecklist);

void UI_ChecklistAddItem(Checklist checklist, char *txt);

void UI_ChecklistDestroy(Checklist checklist);

void UI_checklistToggle(Checklist achecklist, int mouseX, int mouseY);

#endif