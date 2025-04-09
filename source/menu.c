#include "../include/menu.h"
#include "../include/UI/label.h"
#include "../include/UI/button.h"
#include "../include/UI/checklist.h"
#include "../include/UI/inputfield.h"

void renderMenu(SDL_Renderer *pRend, Menu *pMenu) {
    SDL_SetRenderDrawColor(pRend, 0,0,0,0);
    SDL_RenderClear(pRend);
    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelRender(pRend, pMenu->panels[i]);
    }
    SDL_RenderPresent(pRend);
}

void updateMenu(Menu *pMenu, ClientControl *pControl) {
    static MenuEvent menuEvent;
    static int switchDelay = 0;
    switchDelay++;

    menuEvent.isTextInput = pControl->isTextInput;
    if(menuEvent.isTextInput) {
        strcpy(menuEvent.textInput, pControl->textInput);
    }

    pMenu->isGameStarted = false;

    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelUpdate(pMenu->panels[i], &menuEvent, pControl->isMouseUp, pControl->keys);

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
                if (strcmp("New-Game", menuEvent.key) == 0) {
                    pMenu->isGameStarted = true;
                }
                
                break;
        }
    }
}

void refreshMenu(SDL_Renderer *pRend, Menu *pMenu, ClientView *pView) {
    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelSetAppearance(pMenu->panels[i], 
            (SDL_Rect) { .x = 0, .y = 0, .w = pView->windowWidth, .h = pView->windowHeight }, 
            (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }
        );
        UI_panelSetActive(pMenu->panels[i], (i == pMenu->currentPanel));
    }

    // START MENU /////////////////////////
    Button b1 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_START], "Start Game");
    UI_buttonConfigure(b1, "Start Game", pView->windowWidth / 2 - 150, 150, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Button b2 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_START], "Social");
    UI_buttonConfigure(b2, "Social", pView->windowWidth / 2 - 150, 150 + OFFSET, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Button b3 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_START], "Options");
    UI_buttonConfigure(b3, "Options", pView->windowWidth / 2 - 150, 150 + OFFSET*2, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Button b4 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_START], "Quit");
    UI_buttonConfigure(b4, "Quit", pView->windowWidth / 2 - 150, 150 + OFFSET*3, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    // START GAME MENU ////////////////////////
    Button b8 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_GAMESELECTION], "Load-Game");
    UI_buttonConfigure(b8, "Load Game", pView->windowWidth / 2 - 150, 150 + OFFSET, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Button b9 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_GAMESELECTION], "New-Game");
    UI_buttonConfigure(b9, "New Game", pView->windowWidth / 2 - 150, 150 + OFFSET*2, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Button b10 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_GAMESELECTION], "Game-back");
    UI_buttonConfigure(b10, "Back", 
        pView->windowWidth*0.08, pView->windowHeight*0.85, SMALLBUTTONWIDTH, SMALLBUTTONHEIGHT, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    // SOCIAL MENU ////////////////////////
    Inputfield f1 = (Inputfield)UI_panelGetComponent(pMenu->panels[PANEL_SOCIAL], "Social-input");
    UI_inputfieldSetAppearance(pRend, f1, pView->windowWidth / 2 - 150, 150 + OFFSET, BIGBUTTONWIDTH,
        (SDL_Color) {.r = 0, .b = 0, .g = 0, .a = 0}, (SDL_Color) {.r = 255, .b = 255, .g = 255, .a =255}, pMenu->fonts[0]
    );

    Button b5 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_SOCIAL], "Social-back");
    UI_buttonConfigure(b5, "Back", 
        pView->windowWidth*0.08, pView->windowHeight*0.85, SMALLBUTTONWIDTH, SMALLBUTTONHEIGHT, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Button b6 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_SOCIAL], "Join-button");
    UI_buttonConfigure(b6, "Join", pView->windowWidth / 2 - 150, 150 + OFFSET*2, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    // OPTIONS MENU ////////////////////////
    Button b7 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_OPTIONS], "Options-back");
    UI_buttonConfigure(b7, "Back", 
        pView->windowWidth*0.08, pView->windowHeight*0.85, SMALLBUTTONWIDTH, SMALLBUTTONHEIGHT, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
}

Menu initMenu(SDL_Renderer *pRend, ClientView *pView) {
    Menu menu;
    menu.fonts[0] = TTF_OpenFont("assets/fonts/PricedownBl-Regular 900.ttf", 20);
    menu.fonts[1] = TTF_OpenFont("assets/fonts/PricedownBl-Regular 900.ttf", 40);

    menu.currentPanel = PANEL_START;

    for(int i = 0; i < PANEL_COUNT; i++) {
        menu.panels[i] = UI_panelCreate();
    }

    // START MENU /////////////////////////
    UI_panelSetImage(pRend, menu.panels[PANEL_START], "assets/images/menu/menu-background.png");

    Button b1 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_START], b1, UI_BUTTON, "Start Game");
    UI_panelSetComponentLink(menu.panels[PANEL_START], "Start Game", PANEL_GAMESELECTION);

    Button b2 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_START], b2, UI_BUTTON, "Social");
    UI_panelSetComponentLink(menu.panels[PANEL_START], "Social", PANEL_SOCIAL);

    Button b3 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_START], b3, UI_BUTTON, "Options");
    UI_panelSetComponentLink(menu.panels[PANEL_START], "Options", PANEL_OPTIONS);

    Button b4 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_START], b4, UI_BUTTON, "Quit");
    
    // START GAME MENU ////////////////////////
    UI_panelSetImage(pRend, menu.panels[PANEL_GAMESELECTION], "assets/images/menu/background2.png");

    Button b8 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_GAMESELECTION], b8, UI_BUTTON, "Load-Game");
    
    Button b9 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_GAMESELECTION], b9, UI_BUTTON, "New-Game");
    
    Button b10 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_GAMESELECTION], b10, UI_BUTTON, "Game-back");
    UI_panelSetComponentLink(menu.panels[PANEL_GAMESELECTION], "Game-back", PANEL_START);

    // SOCIAL MENU ////////////////////////
    UI_panelSetImage(pRend, menu.panels[PANEL_SOCIAL], "assets/images/menu/background2.png");

    Inputfield f1 = UI_inputfieldCreate();
    UI_panelAddComponent(menu.panels[PANEL_SOCIAL], f1, UI_INPUTFIELD, "Social-input");

    Button b5 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_SOCIAL], b5, UI_BUTTON, "Social-back");
    UI_panelSetComponentLink(menu.panels[PANEL_SOCIAL], "Social-back", PANEL_START);

    Button b6 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_SOCIAL], b6, UI_BUTTON, "Join-button");
    UI_panelSetComponentLink(menu.panels[PANEL_SOCIAL], "Join-button", PANEL_START);

    // OPTIONS MENU ////////////////////////
    UI_panelSetImage(pRend, menu.panels[PANEL_OPTIONS], "assets/images/menu/background3.png");

    Button b7 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_OPTIONS], b7, UI_BUTTON, "Options-back");
    UI_panelSetComponentLink(menu.panels[PANEL_OPTIONS], "Options-back", PANEL_START);

    refreshMenu(pRend, &menu, pView);
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