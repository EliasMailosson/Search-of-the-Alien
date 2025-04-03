#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

typedef struct Button* Button;

/**
 *Is used to create a button comined with a label
*/
Button UI_buttonCreate();

/**
 * Sets custom dimensions for the targeted button
 * \param aButton Targeted button
 * \param newX X-value for the button
 * \param newY Y-value for the button
 * \param newWidth Width for the button
 * \param newHeight Height for the button
*/
void UI_buttonDimensions(Button aButton, int newX, int newY, int newWidth, int newHeight);

/**
 * Changes targeted buttons text
 * \param aButton Targeted button
 * \param text A text pointer, you can directly write in the function with " "
 */
void UI_buttonSetText(Button aButton, char* text);

/**
 * Centers the position of the label and is used in the function "UI_buttonSetLabelappearence"
*/
void UI_buttonSetTextPosition(Button aButton);

/**
 * Sets the appearence of the label in the button
 * \param pRend The renderer to be used
 * \param aButton The Targeted buttons label
 * \param textColor Sets the color of the text
 * \param pfont The specified font for the label inside the button
 * \param buttonColor Changes the buttons color
*/
void UI_buttonSetLabelappearence(SDL_Renderer* pRend, Button aButton, SDL_Color textColor, TTF_Font* pFont, SDL_Color buttonColor);

/**
 * Renders the button in panel
*/
void UI_buttonRenderer(SDL_Renderer* pRend, Button aButton);

/**
 * Gives feedback in a boolean value returned
*/
bool UI_buttonIsHovered(Button aButton, int mouseX, int mouseY);

/**
 * Three functions combined to configure a button
 * \param abutton The targeted button to configure.
 * \param text The text to be displayed on the targeted button.
 * \param newX X-value the button starts on.
 * \param newY Y-value for the button to start on.
 * \param newWidth Total width for the button.
 * \param newHeight Total height of the button.
 * \param prend The renderer to use.
 * \param textColor Color for the label inside the button.
 * \param pFont Which font to use.
 * \param buttonColor Background color for the button.
*/
void UI_buttonConfigure(Button aButton, char* text, int newX, int newY, int newWidth, int newHeight, SDL_Renderer* pRend, SDL_Color textColor, TTF_Font* pFont, SDL_Color buttonColor);

/**
 * Destroys the button and label inside it.
*/
void UI_buttonDestroy(Button aButton);

#endif