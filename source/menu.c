#include "../include/menu.h"
#include "../include/UI/label.h"
#include "../include/UI/button.h"
#include "../include/UI/checklist.h"

void renderMenu(SDL_Renderer *pRend, Menu *pMenu) {
    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelRender(pRend, pMenu->panels[PANEL_START]);
    }
}

void updateMenu(Menu *pMenu, ClientControl *pControl) {
    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelUpdate(pMenu->panels[i], pControl->isMouseUp);
    }
}

Menu initMenu(SDL_Renderer *pRend, ClientView *pView) {
    Menu menu;
    menu.fonts[0] = TTF_OpenFont("assets/fonts/PricedownBl-Regular 900.ttf", 20);
    menu.fonts[1] = TTF_OpenFont("assets/fonts/PricedownBl-Regular 900.ttf", 40);

    menu.currentPanel = PANEL_START;

    for(int i = 0; i < PANEL_COUNT; i++) {
        menu.panels[i] = UI_panelCreate();
        UI_panelSetAppearance(menu.panels[i], 
            (SDL_Rect) { .x = 0, .y = 0, .w = pView->windowWidth, .h = pView->windowHeight }, 
            (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }
        );
    }

    // START MENU /////////////////////////
    Button b1 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_START], b1, UI_BUTTON, "b1");
    UI_buttonSetText(b1, "Start Menu");
    UI_buttonDimensions(b1, 200, 100, 300, 60);
    UI_buttonSetLabelappearence(pRend, b1, 
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        menu.fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    

    // SOCIAL MENU ////////////////////////
    
    return menu;
}

void destroyMenu(Menu *pMenu) {
    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelDestroy(pMenu->panels[i]);
    }

    for(int i = 0; i < FONT_COUNT; i++) {
        TTF_CloseFont(pMenu->fonts[i]);
        pMenu->fonts[i] = NULL;
    }
}