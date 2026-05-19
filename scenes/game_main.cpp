#include "menu.h"
#include "game_main.h"
#include "../structures/InputManager.h"
#include "raylib.h"

void runGame(GameState &currentState, InputManager &input, Font &myFont) {

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
        
        DrawTextEx(myFont, "Stiskni BACKSPACE pro navrat do menu", Vector2{210, 450}, 20.0f, 1.0f, MAROON);

        if (isPaused) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, 0.2f));
            DrawTextEx(myFont, "PAUSED", Vector2{330, 270}, 40.0f, 1.0f, WHITE);
            DrawTextEx(myFont, "Stiskni MEZERNIK pro pokracovani", Vector2{230, 330}, 20.0f, 1.0f, LIGHTGRAY);
        } else {
            DrawTextEx(myFont, "Stiskni MEZERNIK pro pauzu", Vector2{20, 20}, 20.0f, 1.0f, WHITE);
        }
    EndDrawing();
}