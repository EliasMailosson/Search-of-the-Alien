#ifndef PANEL_H
#define PANEL_H

#include <stdbool.h>
#include <SDL.h>

enum CompTypes{UI_BUTTON, UI_LABEL, UI_CHECKLIST, UI_INPUTFIELD};

typedef struct Panel *Panel;

typedef struct menuEvent MenuEvent;

/*Create new empty panel*/
Panel UI_panelCreate();

/*Sets the appearance of a panel*/
void UI_panelSetAppearance(Panel aPanel, SDL_Rect rect, SDL_Color src_bg);

/**
 * Adds a component to the panel.
 * 
 * \param aPanel the panel to add component to.
 * \param comp this is the component. Can hold types: Button, Label, Checklist etc...
 * \param type enum value for the component type. Ex) UI_BUTTON, UI_LABEL
 * \param key unique key (char*) for the component. This is used to check for events linked to specified component.
 * 
*/
void UI_panelAddComponent(Panel aPanel, void *comp, int type, char *key);

/*Renders panel and all child-components*/
void UI_panelRender(SDL_Renderer *pRend, Panel aPanel);

/*Updates all child-components and poll for interaction*/
void UI_panelUpdate(Panel aPanel, MenuEvent *pEvent, bool isMouseUp, bool *keys);

/*Toggle the visibility and activity of a panel*/
void UI_panelSetActive(Panel aPanel, bool active);

/*Adds panel reference link to a button (Must be type Button)*/
void UI_panelSetComponentLink(Panel aPanel, char* key, int panelLink);

void* UI_panelGetComponent(Panel aPanel, char *key);

/*Uses an image as background instead of a color*/
void UI_panelSetImage(SDL_Renderer* pRend, Panel aPanel, char *imagePath);

/*Free's panel and all child-components*/
void UI_panelDestroy(Panel aPanel);

#endif