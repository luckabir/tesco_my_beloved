#include "../main.h"
#include "../structures/InputManager.h"
#include "raylib.h"

void runProfile(GameState &currentState, InputManager &input, Font &myFont) {
    if (input.IsBackTriggered()) {
        currentState = STATE_MENU;
    }

    BeginDrawing();
        ClearBackground(PURPLE); 
        DrawText("PROFILY", 250, 200, 30, WHITE);
    EndDrawing();
}