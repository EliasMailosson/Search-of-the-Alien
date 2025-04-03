#include "../include/menu.h"
#include "../include/UI/label.h"
#include "../include/UI/button.h"
#include "../include/UI/checklist.h"

void renderMenu(SDL_Renderer* pRend, Panel aPanel) {
    UI_panelRender(pRend, aPanel);
}

void updateMenu(Panel aPanel, ClientControl *pControl) {
    UI_panelUpdate(aPanel, pControl->isMouseUp);
}

Panel initMenu(SDL_Renderer* pRend, ClientView *pView) {
    TTF_Font* pFont = TTF_OpenFont("assets/fonts/PricedownBl-Regular 900.ttf", 20);

    Panel aPanel = UI_panelCreate();
    UI_panelSetAppearance(aPanel, 
        (SDL_Rect) { .x = 0, .y = 0, .w = pView->windowWidth, .h = pView->windowHeight }, 
        (SDL_Color) { .r = 0, .g = 200, .b = 255, .a = 255 }
    );

    Label aLabel = UI_labelCreate();
    UI_labelSetAppearance(pRend, aLabel, 100, 100, (SDL_Color) { 255, 255, 255, 255 }, pFont);
    UI_panelAddComponent(aPanel, aLabel, UI_LABEL, "label1");

    Button aButton = UI_buttonCreate();
    UI_panelAddComponent(aPanel, aButton, UI_BUTTON, "button 1");
    UI_buttonDimensions(aButton, 700, 200, 50, 100);
    UI_buttonSetLabelappearence(pRend, aButton, (SDL_Color) { 255, 255, 255, 255 }, pFont, (SDL_Color) {0, 0, 0, 0});

    Checklist aChecklist = UI_checklistCreate();
    UI_panelAddComponent(aPanel, aChecklist, UI_CHECKLIST, "Checklist");

    return aPanel;
}