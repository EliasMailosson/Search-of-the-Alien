#include "../include/menu.h"

void renderMenu(SDL_Renderer *pRend, Panel aPanel) {
    UI_panelRender(pRend, aPanel);
}

Panel initMenu() {
    Panel aPanel = UI_panelCreate();
    UI_panelSetAppearance(aPanel, (SDL_Rect){.x=134,.y=0,.w=100,.h=400}, (SDL_Color){.r=0, .g=200, .b=255, .a=255});

    return aPanel;
}