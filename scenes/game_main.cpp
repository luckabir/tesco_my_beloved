#include "menu.h"
#include "game_main.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h"
#include "raylib.h"

void runGame(GameState &currentState, InputManager &input) {

    static bool isPaused = false;
;
    if (input.IsBackTriggered()) {
        isPaused = false; 
        currentState = STATE_MENU;
        return;
    }

    if (input.IsPauseTriggered()) {
        isPaused = !isPaused;
    }

    BeginDrawing();
        ClearBackground(BLUE); 

        DrawText("--- SAMOTNA HRA ---", 280, 200, 30, WHITE);
        DrawText("Tady pobezi tvoje herni logika.", 250, 280, 20, LIGHTGRAY);
        
        if (isPaused) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.2f));
            DrawTextEx(AssetManager::mainFont, "PAUZNUTO", Vector2{310, 270}, 40.0f, 1.0f, WHITE);
            DrawTextEx(AssetManager::mainFont, "STISKNI MEZERNIK PRO POKRACOVANI", Vector2{170, 340}, 20.0f, 1.0f, LIGHTGRAY);
        }

    EndDrawing();
}