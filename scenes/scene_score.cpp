#include "../main.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h" 
#include "raylib.h"

void runScore(GameState &currentState, InputManager &input) {
    if (input.IsBackTriggered()) {
        currentState = STATE_MENU;
    }

    BeginDrawing();
        ClearBackground(PURPLE); 
        DrawText("ZEBŘÍČEK NEJLEPŠÍCH", 250, 200, 30, WHITE);
    EndDrawing();
}