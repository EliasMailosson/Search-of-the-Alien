#ifndef TERMINALHUB_H
#define TERMINALHUB_H

#include "UI/button.h"
#include "game.h"

void renderTerminalHub(ClientView *pView, TerminalHub planetchooser);
void initTerminalHub(ClientView *pView, TerminalHub *pTerminalHub);
void destroyTerminalHub(TerminalHub *pTerminalHub);
void updateTerminalHub(TerminalHub *pTerminalHub, Client aClient, bool isMouseUp);
void refreshTerminalHub(ClientView *pView, TerminalHub *pTerminalHub);

#endif