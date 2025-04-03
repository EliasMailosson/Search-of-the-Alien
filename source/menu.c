#include "../include/menu.h"
#include "../include/UI/label.h"
#include "../include/UI/button.h"
#include "../include/UI/checklist.h"

void renderMenu(SDL_Renderer *pRend, Menu *pMenu) {
    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelRender(pRend, pMenu->panels[i]);
    }
}

void updateMenu(Menu *pMenu, ClientControl *pControl) {
    static MenuEvent menuEvent;
    static int switchDelay = 0;
    switchDelay++;
    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelUpdate(pMenu->panels[i], &menuEvent, pControl->isMouseUp);

        switch(menuEvent.eventType) {
            case PANEL_SWITCH:
                if(pMenu->currentPanel != menuEvent.newPanel && switchDelay > 4) {
                    pMenu->currentPanel = menuEvent.newPanel;
                    for(int i = 0; i < PANEL_COUNT; i++) {
                        UI_panelSetActive(pMenu->panels[i], (i == pMenu->currentPanel));
                    }
                    switchDelay = 0;
                }
                break;
        }
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
        UI_panelSetActive(menu.panels[i], (i == menu.currentPanel));
    }

    // START MENU /////////////////////////
    UI_panelSetImage(pRend, menu.panels[PANEL_START], "assets/images/menu-background.png");

    Button b1 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_START], b1, UI_BUTTON, "b1");
    UI_buttonSetText(b1, "Start Menu");
    UI_buttonDimensions(b1, 200, 100, 300, 60);
    UI_buttonSetLabelappearence(pRend, b1, 
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        menu.fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    UI_panelSetComponentLink(menu.panels[PANEL_START], "b1", PANEL_SOCIAL);

    // SOCIAL MENU ////////////////////////
    UI_panelSetImage(pRend, menu.panels[PANEL_SOCIAL], "assets/images/menu-background.png");

    Button b2 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_SOCIAL], b2, UI_BUTTON, "b2");
    UI_buttonConfigure(b2, "Social", 
        200, 100, 300, 60, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        menu.fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    // UI_buttonSetText(b2, "Social");
    // UI_buttonDimensions(b2, 200, 100, 300, 60);
    // UI_buttonSetLabelappearence(pRend, b2, 
    //     (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
    //     menu.fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    // );
    UI_panelSetComponentLink(menu.panels[PANEL_SOCIAL], "b2", PANEL_START);

    // OPTIONS MENU ////////////////////////
    
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