#include "../include/menu.h"
#include "../include/UI/label.h"
#include "../include/UI/button.h"
#include "../include/UI/checklist.h"
#include "../include/UI/inputfield.h"

#define OFFSET 80

void renderMenu(SDL_Renderer *pRend, Menu *pMenu) {
    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelRender(pRend, pMenu->panels[i]);
    }
}

void updateMenu(Menu *pMenu, ClientControl *pControl) {
    static MenuEvent menuEvent;
    static int switchDelay = 0;
    switchDelay++;

    menuEvent.isTextInput = pControl->isTextInput;
    if(menuEvent.isTextInput) {
        strcpy(menuEvent.textInput, pControl->textInput);
    }

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
            case BUTTON_CLICKED:
                if (strcmp("Quit", menuEvent.key) == 0) {
                    pControl->isRunning = false;
                    return;
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
    UI_panelAddComponent(menu.panels[PANEL_START], b1, UI_BUTTON, "Start Game");
    UI_buttonConfigure(b1, "Start Game", pView->windowWidth / 2 - 150, 150, 300, 60, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        menu.fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    UI_panelSetComponentLink(menu.panels[PANEL_START], "Start Game", PANEL_SOCIAL);

    Button b2 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_START], b2, UI_BUTTON, "Social");
    UI_buttonConfigure(b2, "Social", pView->windowWidth / 2 - 150, 150 + OFFSET, 300, 60, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        menu.fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    UI_panelSetComponentLink(menu.panels[PANEL_START], "Social", PANEL_SOCIAL);

    Button b3 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_START], b3, UI_BUTTON, "Options");
    UI_buttonConfigure(b3, "Options", pView->windowWidth / 2 - 150, 150 + OFFSET*2, 300, 60, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        menu.fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    UI_panelSetComponentLink(menu.panels[PANEL_START], "Options", PANEL_SOCIAL);

    Button b4 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_START], b4, UI_BUTTON, "Quit");
    UI_buttonConfigure(b4, "Quit", pView->windowWidth / 2 - 150, 150 + OFFSET*3, 300, 60, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        menu.fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    // SOCIAL MENU ////////////////////////
    UI_panelSetImage(pRend, menu.panels[PANEL_SOCIAL], "assets/images/menu-background.png");

    Button b5 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_SOCIAL], b5, UI_BUTTON, "b5");
    UI_buttonConfigure(b5, "Social", 
        200, 100, 300, 60, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        menu.fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    UI_panelSetComponentLink(menu.panels[PANEL_SOCIAL], "b5", PANEL_START);

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