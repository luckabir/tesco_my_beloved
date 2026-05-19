#include "menu.h"
#include "scene_settings.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h" 
#include "raylib.h"


void runSettings(GameState &currentState, InputManager &input) {

    if (input.IsBackTriggered()) {
        currentState = STATE_MENU;
    }

    BeginDrawing();
        ClearBackground(DARKGREEN);

        DrawText("--- NASTAVENI HRY ---", 260, 200, 30, WHITE);
        DrawText("Zde budes moct menit hlasitost nebo grafiku.", 190, 280, 20, LIGHTGRAY);
        
        DrawText("Stiskni BACKSPACE pro navrat do menu", 210, 450, 20, LIME);
    EndDrawing();
}