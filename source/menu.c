#include "../include/menu.h"
#include "../include/UI/label.h"
#include "../include/UI/button.h"
#include "../include/UI/checklist.h"
#include "../include/UI/inputfield.h"
#include "../include/UI/friend.h"
#include "../include/UI/animation.h"
#include "../include/players.h"
#include "../include/NET/client.h"
#include "../include/game.h"
#include <stdio.h>
#include <string.h>

void renderMenu(SDL_Renderer *pRend, Menu *pMenu) {
    SDL_SetRenderDrawColor(pRend, 0,0,0,0);
    SDL_RenderClear(pRend);
    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelRender(pRend, pMenu->panels[i]);
    }

    SDL_RenderPresent(pRend);
}

void updateMenu(Menu *pMenu, ClientControl *pControl, ClientView *pView, Client aClient, Sound aSound) {
    static MenuEvent menuEvent;
    static int switchDelay = 0;
    switchDelay++;

    menuEvent.isTextInput = pControl->isTextInput;
    if(menuEvent.isTextInput) {
        strcpy(menuEvent.textInput, pControl->textInput);
    }

    pMenu->isGameStarted = false;
    strcpy(menuEvent.key, "");

    // behöver dubbel kollas med UI gänget om det är hållbart istället för en for loop
    UI_panelUpdate(pMenu->panels[pMenu->currentPanel], &menuEvent, pControl->isMouseUp, pControl->keys, aSound);

        switch(menuEvent.eventType) {
            case PANEL_SWITCH:
                if(pMenu->currentPanel != menuEvent.newPanel && switchDelay > 4) {
                    pMenu->currentPanel = menuEvent.newPanel;
                    for(int i = 0; i < PANEL_COUNT; i++) {
                        UI_panelSetActive(pMenu->panels[i], (i == pMenu->currentPanel));
                    }
                    switchDelay = 0;
                }
                if (strcmp("MyUsername", menuEvent.key) == 0){
                    char myUsername[MAX_USERNAME_LEN];
                    createNewUsername(pMenu, myUsername);
                    NET_clientSetSelfName(aClient, myUsername);
                    NET_clientSendString(aClient,MENU,CONNECT,myUsername);
                }
                if (strcmp("AddFriend-Friends", menuEvent.key) == 0 || strcmp("Friends-button", menuEvent.key) == 0 || strcmp("Friends", menuEvent.key) == 0) {
                
                    updateFriendList(pMenu, aClient);
                }
                if (strcmp("player-sel", menuEvent.key) == 0) {
                    Animation a1 = (Animation)UI_panelGetComponent(pMenu->panels[PANEL_PLAYER_SELECT], "player-animation");
                    pControl->selectedCharacter = UI_animationGetRow(a1) % MAX_PLAYER_CHARACTERS;
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
                if (strcmp("Join-Friend", menuEvent.key) == 0) {
                    
                    printf("Join friend clicked!\n");
                    pMenu->isGameStarted = true;
                }
                if (strcmp("AddFriend-Add", menuEvent.key) == 0){
                    char friend[MAX_USERNAME_LEN];
                    FriendList aFriendList = (FriendList)UI_panelGetComponent(pMenu->panels[PANEL_FRIENDS],"social-friendlist");
                    addFriendList(pMenu, friend);
                    UI_clientAddFriend(aFriendList, friend);
                    UI_updateFriendList(aFriendList);
                }
                if (strcmp("Options-quality-sel", menuEvent.key) == 0) {
                    int current = NET_clientGetNextGraphicsConfig(aClient);
                    if(current == 2) NET_clientSetNextGraphicsConfig(aClient, 1);
                    if(current == 1) NET_clientSetNextGraphicsConfig(aClient, 2);
                    refreshMenu(aClient, pView->pRend, pMenu, pView);
                }
                if (strcmp("player-r", menuEvent.key) == 0) {
                    Animation a1 = (Animation)UI_panelGetComponent(pMenu->panels[PANEL_PLAYER_SELECT], "player-animation");
                    UI_animationSetMode(a1, PLAYBACK_FORWARD);
                    
                }
                else if (strcmp("player-l", menuEvent.key) == 0) {
                    Animation a1 = (Animation)UI_panelGetComponent(pMenu->panels[PANEL_PLAYER_SELECT], "player-animation");
                    UI_animationSetMode(a1, PLAYBACK_BACKWARD);
                }
                break;
        }
}

void updateFriendList(Menu *pMenu, Client aClient) {
    FriendList aFriendList = (FriendList)UI_panelGetComponent(pMenu->panels[PANEL_FRIENDS], "social-friendlist");
    
    UI_readFriendList(aFriendList);
    UI_SetFriendsOffline(aFriendList);

    char outputNames[MAX_FRIENDS][MAX_USERNAME_LEN];
    int PlayerCount = NET_clientGetPlayerCount(aClient);
    NET_clientGetFriendsName(aClient, outputNames);

    for (int i = 0; i < PlayerCount; i++) {
        UI_FriendremoveSpacesInBetween(outputNames[i]);
        UI_friendListSetStatus(aFriendList, outputNames[i]);
    }
}

void refreshMenu(Client aClient, SDL_Renderer *pRend, Menu *pMenu, ClientView *pView) {
    for(int i = 0; i < PANEL_COUNT; i++) {
        UI_panelSetAppearance(pMenu->panels[i], 
            (SDL_Rect) { .x = 0, .y = 0, .w = pView->windowWidth, .h = pView->windowHeight }, 
            (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }
        );
        UI_panelSetActive(pMenu->panels[i], (i == pMenu->currentPanel));
    }

    // INPUT MY USERNAME /////////////////////////
    Inputfield f3 = (Inputfield)UI_panelGetComponent(pMenu->panels[PANEL_MYUSERNAME], "MyUsername-input");
    UI_inputfieldSetAppearance(pRend, f3, pView->windowWidth / 2 - 150, 260, BIGBUTTONWIDTH,
        (SDL_Color){0,0,0,255}, (SDL_Color){255,255,255,255}, pMenu->fonts[1]);

    Button b17 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_MYUSERNAME], "MyUsername");
    UI_buttonConfigure(b17, "Submit Username", pView->windowWidth / 2 - 200, 250 + OFFSET, 400, BIGBUTTONHEIGHT,
        pRend, (SDL_Color){0,0,0,255}, pMenu->fonts[1], (SDL_Color){255,255,255,255});

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

    Button b5 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_SOCIAL], "Social-back"); // "back" knappen
    UI_buttonConfigure(b5, "Back", 
        pView->windowWidth*0.08, pView->windowHeight*0.85, SMALLBUTTONWIDTH, SMALLBUTTONHEIGHT, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Button b6 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_SOCIAL], "AddFriend-button"); 
    UI_buttonConfigure(b6, "Add Friend", pView->windowWidth / 2 - 150, 150 + OFFSET*1, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 } 
    );

    Button b11 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_SOCIAL],"Friends-button");
    UI_buttonConfigure(b11,"Friends", pView->windowWidth / 2 - 150, 150 + OFFSET*2, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pRend,
        (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[1], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    //FRIENDS MENU ////////////////////////
    Button b12 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_FRIENDS], "Friends-back");
    UI_buttonConfigure(b12, "Back", pView->windowWidth * 0.08, pView->windowHeight * 0.85, SMALLBUTTONWIDTH, SMALLBUTTONHEIGHT,
        pRend, (SDL_Color){.r = 0, .g = 0, .b = 0, .a = 255}, pMenu->fonts[0],
        (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255});

    Button b16 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_FRIENDS], "Friends-AddFriend");
    UI_buttonConfigure(b16,"Add Friend", pView->windowWidth * 0.8, pView->windowHeight * 0.85, SMALLBUTTONWIDTH, SMALLBUTTONHEIGHT,
        pRend, (SDL_Color){.r = 0, .g = 0, .b = 0, .a = 255}, pMenu->fonts[0],
        (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255});
    
    Button b170 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_FRIENDS], "Join-Friend");
    UI_buttonConfigure(b170, "Join", pView->windowWidth * 0.8, pView->windowHeight * 0.15,
        SMALLBUTTONWIDTH, SMALLBUTTONHEIGHT, pRend,
        (SDL_Color){0, 0, 0, 255},
        pMenu->fonts[1],
        (SDL_Color){255, 255, 255, 255}
    );
    //ADDFRIENDS MENU /////////////////////
    Inputfield f2 = (Inputfield)UI_panelGetComponent(pMenu->panels[PANEL_ADDFRIEND], "AddFriend-input");
    UI_inputfieldSetAppearance(pRend, f2, pView->windowWidth / 2 - 150, 150 + OFFSET,
        BIGBUTTONWIDTH, (SDL_Color){.r = 0, .b = 0, .g = 0, .a = 255}, 
        (SDL_Color){.r = 255, .b = 255, .g = 255, .a =255}, pMenu->fonts[0]);
    
    Button b14 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_ADDFRIEND], "AddFriend-Add");
    UI_buttonConfigure(b14, "Add Friend", pView->windowWidth / 2 - 150, 150 + OFFSET*2, BIGBUTTONWIDTH, BIGBUTTONHEIGHT, pRend,
        (SDL_Color){.r = 0, .g = 0, .b = 0, .a = 255}, pMenu->fonts[1],
        (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255});
    
    Button b13 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_ADDFRIEND], "AddFriend-back");
    UI_buttonConfigure(b13, "Back", pView->windowWidth * 0.08, pView->windowHeight * 0.85, SMALLBUTTONWIDTH, SMALLBUTTONHEIGHT,
        pRend, (SDL_Color){.r = 0, .g = 0, .b = 0, .a = 255}, pMenu->fonts[0],
        (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255});

    Button b15 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_ADDFRIEND], "AddFriend-Friends");
    UI_buttonConfigure(b15,"Friends", pView->windowWidth * 0.8, pView->windowHeight * 0.85, SMALLBUTTONWIDTH, SMALLBUTTONHEIGHT,
        pRend, (SDL_Color){.r = 0, .g = 0, .b = 0, .a = 255}, pMenu->fonts[0],
        (SDL_Color){.r = 255, .g = 255, .b = 255, .a = 255});


    // OPTIONS MENU ////////////////////////
    Button b7 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_OPTIONS], "Options-back");
    UI_buttonConfigure(b7, "Back", 
        pView->windowWidth*0.08, pView->windowHeight*0.85, SMALLBUTTONWIDTH, SMALLBUTTONHEIGHT, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Button b71 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_OPTIONS], "Options-player-sel");
    UI_buttonConfigure(b71, "Choose Your Character", 
        100, 100, BIGBUTTONWIDTH, SMALLBUTTONHEIGHT, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
    
    char graphicsText[64];
    int quality = NET_clientGetNextGraphicsConfig(aClient);
    if(quality == 1) strcpy(graphicsText, "Graphics Quality: High");
    else if(quality == 2) strcpy(graphicsText, "Graphics Quality: Medium");
    Button b50 = (Button)UI_panelGetComponent(pMenu->panels[PANEL_OPTIONS], "Options-quality-sel");
    UI_buttonConfigure(b50, graphicsText, 
        100, 150, BIGBUTTONWIDTH, SMALLBUTTONHEIGHT, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Label l3 = (Label)UI_panelGetComponent(pMenu->panels[PANEL_OPTIONS], "Options-quality-disclaimer");
    UI_labelSetText(l3, "(NOTE: Restart game for change to take effect!)");
    UI_labelSetAppearance(pView->pRend, l3, 450, 150, (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }, pMenu->fonts[0]);

    // PANEL_PLAYER_SELECT //////////////////
    Animation a1 = (Animation)UI_panelGetComponent(pMenu->panels[PANEL_PLAYER_SELECT], "player-animation");
    UI_animationSetDestination(a1, (SDL_Rect) {.x = pView->windowWidth/4, .y = pView->windowHeight/4, .w = pView->windowWidth/2, .h = pView->windowHeight/2});

    Button bLeft = (Button)UI_panelGetComponent(pMenu->panels[PANEL_PLAYER_SELECT], "player-l");
    UI_buttonConfigure(bLeft, "<-", 
        40, pView->windowHeight*0.85, 40, SMALLBUTTONHEIGHT, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Button bRight = (Button)UI_panelGetComponent(pMenu->panels[PANEL_PLAYER_SELECT], "player-r");
    UI_buttonConfigure(bRight, "->", 
        pView->windowWidth - 80, pView->windowHeight*0.85, 40, SMALLBUTTONHEIGHT, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );

    Button bSel = (Button)UI_panelGetComponent(pMenu->panels[PANEL_PLAYER_SELECT], "player-sel");
    UI_buttonConfigure(bSel, "Confirm", 
        pView->windowWidth/2 - 80, pView->windowHeight*0.85, 160, SMALLBUTTONHEIGHT, pRend, (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 255 }, 
        pMenu->fonts[0], (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 }
    );
}
//CreateFriendList() och panelAddComponent
Menu initMenu(SDL_Renderer *pRend, ClientView *pView, Client aClient) {
    Menu menu;
    menu.fonts[0] = TTF_OpenFont("assets/fonts/PricedownBl-Regular 900.ttf", 20);
    menu.fonts[1] = TTF_OpenFont("assets/fonts/PricedownBl-Regular 900.ttf", 40);
    menu.fonts[2] = TTF_OpenFont("assets/fonts/PricedownBl-Regular 900.ttf", 60);
    menu.fonts[3] = TTF_OpenFont("assets/fonts/jura.ttf", 20);

    pView->fonts = menu.fonts[3];

    menu.currentPanel = PANEL_START;

    for(int i = 0; i < PANEL_COUNT; i++) {
        menu.panels[i] = UI_panelCreate();
    }

    // INPUT USERNAME /////////////////////////

    UI_panelSetImage(pRend, menu.panels[PANEL_MYUSERNAME], "assets/images/menu/background_username.png");

    Inputfield f3 = UI_inputfieldCreate();
    UI_panelAddComponent(menu.panels[PANEL_MYUSERNAME], f3, UI_INPUTFIELD, "MyUsername-input");

    Button b17 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_MYUSERNAME], b17, UI_BUTTON, "MyUsername");
    UI_panelSetComponentLink(menu.panels[PANEL_MYUSERNAME], "MyUsername", PANEL_START);

    checkUsername(&menu, aClient);

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

    Button b5 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_SOCIAL], b5, UI_BUTTON, "Social-back");
    UI_panelSetComponentLink(menu.panels[PANEL_SOCIAL], "Social-back", PANEL_START);

    Button b6 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_SOCIAL], b6, UI_BUTTON, "AddFriend-button");
    UI_panelSetComponentLink(menu.panels[PANEL_SOCIAL], "AddFriend-button", PANEL_ADDFRIEND);

    Button b11 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_SOCIAL], b11, UI_BUTTON, "Friends-button");
    UI_panelSetComponentLink(menu.panels[PANEL_SOCIAL], "Friends-button", PANEL_FRIENDS); 

    //FRIENDS MENU ////////////////////////
    UI_panelSetImage(pRend,menu.panels[PANEL_FRIENDS], "assets/images/menu/background2.png");


    FriendList aFriendList = UI_friendListCreate(menu.fonts[2]);
    UI_panelAddComponent(menu.panels[PANEL_FRIENDS], aFriendList, UI_FRIENDLIST, "social-friendlist");

    Button b12 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_FRIENDS], b12, UI_BUTTON, "Friends-back");
    UI_panelSetComponentLink(menu.panels[PANEL_FRIENDS], "Friends-back", PANEL_SOCIAL);

    Button b16 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_FRIENDS],b16, UI_BUTTON, "Friends-AddFriend");
    UI_panelSetComponentLink(menu.panels[PANEL_FRIENDS],"Friends-AddFriend",PANEL_ADDFRIEND);

    Button b170 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_FRIENDS], b170, UI_BUTTON, "Join-Friend");

    //ADDFRIENDS MENU ////////////////////////
    UI_panelSetImage(pRend, menu.panels[PANEL_ADDFRIEND], "assets/images/menu/background2.png");

    Inputfield f2 = UI_inputfieldCreate();
    UI_panelAddComponent(menu.panels[PANEL_ADDFRIEND], f2, UI_INPUTFIELD, "AddFriend-input");
    
    Button b13 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_ADDFRIEND], b13, UI_BUTTON, "AddFriend-back");
    UI_panelSetComponentLink(menu.panels[PANEL_ADDFRIEND], "AddFriend-back", PANEL_SOCIAL);

    Button b14 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_ADDFRIEND], b14, UI_BUTTON, "AddFriend-Add");

    Button b15 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_ADDFRIEND], b15, UI_BUTTON, "AddFriend-Friends");
    UI_panelSetComponentLink(menu.panels[PANEL_ADDFRIEND], "AddFriend-Friends",PANEL_FRIENDS);

    // OPTIONS MENU ////////////////////////
    UI_panelSetImage(pRend, menu.panels[PANEL_OPTIONS], "assets/images/menu/background3.png");

    Button b7 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_OPTIONS], b7, UI_BUTTON, "Options-back");
    UI_panelSetComponentLink(menu.panels[PANEL_OPTIONS], "Options-back", PANEL_START);

    Button b71 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_OPTIONS], b71, UI_BUTTON, "Options-player-sel");
    UI_panelSetComponentLink(menu.panels[PANEL_OPTIONS], "Options-player-sel", PANEL_PLAYER_SELECT);

    Button b50 = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_OPTIONS], b50, UI_BUTTON, "Options-quality-sel");

    Label l3 = UI_labelCreate();
    UI_panelAddComponent(menu.panels[PANEL_OPTIONS], l3, UI_LABEL, "Options-quality-disclaimer");

    // PANEL_PLAYER_SELECT //////////////////
    UI_panelSetImage(pRend, menu.panels[PANEL_PLAYER_SELECT], "assets/images/menu/background2.png");

    Animation a1 = UI_animationCreate();
    UI_panelAddComponent(menu.panels[PANEL_PLAYER_SELECT], a1, UI_ANIMATION, "player-animation");
    UI_animationLoad(a1, pView->pRend, "assets/images/menu/character_selection.png", 960, 512, 72);

    Button bLeft = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_PLAYER_SELECT], bLeft, UI_BUTTON, "player-l");

    Button bRight = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_PLAYER_SELECT], bRight, UI_BUTTON, "player-r");

    Button bSel = UI_buttonCreate();
    UI_panelAddComponent(menu.panels[PANEL_PLAYER_SELECT], bSel, UI_BUTTON, "player-sel");
    UI_panelSetComponentLink(menu.panels[PANEL_PLAYER_SELECT], "player-sel", PANEL_START);

    refreshMenu(aClient,pRend, &menu, pView);
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

void checkUsername(Menu *pMenu, Client aClient){
    FILE *fp;
    fp = fopen("data/myUsername.txt", "r");
    if (fp == NULL)
    {
        pMenu->currentPanel = PANEL_MYUSERNAME;
    }
    else
    {
        char username[MAX_USERNAME_LEN];
        fgets(username, MAX_USERNAME_LEN, fp);
        pMenu->currentPanel = PANEL_START;
        NET_clientSetSelfName(aClient, username);
    }
    fclose(fp);
}

void createNewUsername(Menu *pMenu, char *output){
    Inputfield input = UI_panelGetComponent(pMenu->panels[PANEL_MYUSERNAME], "MyUsername-input");
    UI_inputfieldGetBuffer(input, output);
    FILE *fp ;
    fp = fopen("data/myUsername.txt", "w");
    if (fp != NULL)
    {
        fprintf(fp, "%s", output);
    }

    fclose(fp);
}

void addFriendList(Menu *pMenu, char *output){
    Inputfield input = UI_panelGetComponent(pMenu->panels[PANEL_ADDFRIEND], "AddFriend-input");
    UI_inputfieldGetBuffer(input, output);
}