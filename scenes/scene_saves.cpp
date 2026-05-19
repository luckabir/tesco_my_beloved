#include "../main.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h" 
#include "raylib.h"

void runSaves(GameState &currentState, InputManager &input) {
    if (input.IsBackTriggered()) {
        currentState = STATE_MENU;
    }

        ClearBackground(PURPLE); 
        DrawText("ULOŽENÉ HRY", 250, 200, 30, WHITE);
}