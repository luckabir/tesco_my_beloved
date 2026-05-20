#include "menu.h"
#include "game_main.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h"
#include "raylib.h"

void runGame(GameState &currentState, InputManager &input, bool& isGamePaused) {

    // Aby ti zmáčknutí ESC náhodou nevypínalo celou hru, řekneme Raylibu,
    // že ESC nemá žádnou speciální ukončovací funkci.
    SetExitKey(KEY_NULL);

    // --- POZICE PRO NÁŠ POHYBLIVÝ KOŠÍK ---
    static float basketX = 10.0f;
    static float basketY = 400.0f;
    static float speed = 2.0f;

    // --- MEZERNÍK TĚ HOĎÍ DO MENU A PAUZNE HRU ---
    // Místo in-game pauzy tě tohle rovnou teleportuje do hlavního menu
    if (input.IsPauseTriggered()) { 
        isGamePaused = true;       // Nastavíme stav pauzy pro menu
        currentState = STATE_MENU;  // Přepneme scénu do menu
        return;
    }

    // Kdyby ses chtěla vracet do menu i nějakou jinou klávesou (třeba tím ESC, pokud ho odchytáváš v InputManageru)
    if (input.IsBackTriggered()) {
        isGamePaused = true;
        currentState = STATE_MENU;
        return;
    }

    // =================================================================
    // GLOBÁLNÍ HERNÍ LOGIKA (Běží, jen když NENÍ pauza)
    // =================================================================
    if (!isGamePaused) {
        // Posuneme nákupní košík dopředu
        basketX += speed;

        // Když košík dojede na konec pásu (k pokladně), vrátíme ho zpět na začátek
        if (basketX > 700.0f) {
            basketX = 10.0f;
        }
    }

    // =================================================================
    // VYKRESLOVÁNÍ (Kreslíme vždy do virtuálního okna 800x600)
    // =================================================================
    ClearBackground(BLUE); 

    // Nadpisy
    DrawTextEx(AssetManager::mainFont, "--- TESCO BRIGADA ---", Vector2{260, 50}, 24.0f, 1.0f, YELLOW);
    DrawTextEx(AssetManager::mainFont, "STISKNI MEZERNIK PRO NAVRAT DO MENU", Vector2{210, 100}, 14.0f, 1.0f, LIGHTGRAY);
    
    // --- VYKRESLENÍ POHYBLIVÉHO PÁSU ---
    DrawRectangle(0, 450, 800, 40, DARKGRAY);
    for (int i = 0; i < 800; i += 40) {
        DrawRectangle(i, 450, 4, 40, BLACK);
    }

    // --- VYKRESLENÍ NÁKUPNÍHO KOŠÍKU ---
    DrawRectangle((int)basketX, (int)basketY, 80, 50, ORANGE); 
    DrawRectangle((int)basketX + 15, (int)basketY + 10, 50, 30, RAYWHITE); 
    DrawTextEx(AssetManager::mainFont, "NAKUP", Vector2{basketX + 22, basketY + 20}, 12.0f, 1.0f, BLACK);

    // --- VYKRESLENÍ POKLADNY ---
    DrawRectangle(720, 350, 80, 140, GRAY);
    DrawRectangle(740, 380, 40, 30, GREEN); 
}