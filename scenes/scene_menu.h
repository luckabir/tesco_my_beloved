#ifndef MENU_H
#define MENU_H

#include "../main.h"

void runMenu(GameState &currentState, InputManager &input, bool& isGamePaused);
bool HasSaveGame();
void DrawBouncingTescoLogo(int startX, int startY, int logoSize);

#endif