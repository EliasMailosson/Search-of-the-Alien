#ifndef TERMINALHUB_H
#define TERMINALHUB_H

#include "UI/button.h"
#include "game.h"

void renderTerminalHub(ClientView *pView, TerminalHub planetchooser);
TerminalHub initTerminalHub(ClientView *pView);
void destroyTerminalHub(TerminalHub *pTerminalHub);
void updateTerminalHub(TerminalHub *pTerminalHub, Client aClient, bool isMouseUp);

#endif