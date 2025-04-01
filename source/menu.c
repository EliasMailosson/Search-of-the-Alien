#include "../include/menu.h"
#include "../include/UI/label.h"

void renderMenu(SDL_Renderer *pRend, Panel aPanel) {
    UI_panelRender(pRend, aPanel);
}

Panel initMenu(SDL_Renderer *pRend) {
    TTF_Font *pFont = TTF_OpenFont("assets/fonts/PricedownBl-Regular 900.ttf", 20);

    Panel aPanel = UI_panelCreate();
    UI_panelSetAppearance(aPanel, (SDL_Rect){.x=134,.y=0,.w=100,.h=400}, (SDL_Color){.r=0, .g=200, .b=255, .a=255});

    Label aLabel = UI_labelCreate();
    UI_labelSetAppearance(pRend, aLabel, 100, 100, (SDL_Color){255,255,255,255}, pFont);
    UI_panelAddComponent(aPanel, aLabel, UI_LABEL, "label1");

    return aPanel;
}