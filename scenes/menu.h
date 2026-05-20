#ifndef MENU_H
#define MENU_H

#include "../main.h"


bool HasSaveGame();
void runMenu(GameState &currentState, InputManager &input, bool& isGamePaused);
void DrawBouncingTescoLogo(int startX, int startY, int logoSize);

#endif