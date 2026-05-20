#include "../main.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h" 
#include "raylib.h"

void runIntro(GameState &currentState, InputManager &input) {
    // Statické proměnné pro řízení fází animace
    static int phase = 0;             // 0 = objevování loga, 1 = zatmění do čarene, 2 = roztmívání menu
    static float logoAlpha = 0.0f;    // Průhlednost loga (0.0 až 1.0)
    static float blackAlpha = 0.0f;   // Průhlednost černé opony pro přechod
    static float shakeOffset = 0.0f;  // Efekt otřesu při "dopadu"

    // Možnost přeskočit intro mezerníkem
    if (input.IsPauseTriggered() || input.IsBackTriggered()) {
        currentState = STATE_MENU;
        return;
    }

    // --- LOGIKA ANIMACE (TVŮJ NOVÝ COOL PLÁN) ---
    if (phase == 0) {
        // FÁZE 0: Logo Tesca se pomalu vynořuje ze tmy
        logoAlpha += 0.015f; 
        if (logoAlpha >= 1.0f) {
            logoAlpha = 1.0f;
            shakeOffset = 10.0f; // Krátký otřes obrazovky při plném zjevení
            phase = 1;           // Jdeme na zatmění
        }
    }
   else if (phase == 1) {
        // FÁZE 1: Pomalu se stmívá do černé (Fade Out)
        if (shakeOffset > 0) shakeOffset -= 0.5f; 
        
        blackAlpha += 0.02f; 
        if (blackAlpha >= 1.0f) {
            blackAlpha = 1.0f;
            
            static float blackScreenTimer = 0.0f;
            blackScreenTimer += GetFrameTime(); // Přičítá čas v sekundách od posledního snímku
            
            if (blackScreenTimer >= 1.0f) { // Až přejde 1 sekunda...
                phase = 2; 
                currentState = STATE_MENU; 
                blackScreenTimer = 0.0f; // Reset časovače pro příště
            }
        }
    }


    // --- VYKRESLOVÁNÍ INTRA ---
        
        // Pokud jsme ve fázi 2, vykreslíme na pozadí už samotné menu, aby se pod oponou plynule objevilo!
        if (phase == 2) {
            ClearBackground(RAYWHITE);
            int logoX = 312; int logoY = 110; int logoSize = 35;
            
            // Malé logo v menu pomocí Daydream fontu (OPRAVENO NA ASSETMANAGER)
            DrawTextEx(AssetManager::mainFont, "TESCO", Vector2{(float)logoX, (float)logoY}, (float)logoSize, 2.0f, BLUE);
            
            // BEZPEČNÉ PODTRŽENÍ: Kreslení malých červených obdélníčků pod písmena v menu (velikost 35)
            // Protože Daydream je pixelový font, zvětšili jsme šířku písmene na 0.75f, aby čárky seděly pod blocky
            float menuLetterWidth = logoSize * 0.75f;  
            float menuDashWidth = menuLetterWidth * 0.7f; 
            float menuDashHeight = logoSize * 0.15f;  
            for (int i = 0; i < 5; i++) {
                int dashX = logoX + (i * menuLetterWidth) + (menuLetterWidth - menuDashWidth) / 2;
                int dashY = logoY + logoSize + 2;
                DrawRectangle(dashX, dashY, (int)menuDashWidth, (int)menuDashHeight, RED);
            }
            
            // Provizorní vykreslení tvých šedých tlačítek na správných pozicích pro plynulý efekt
            for (int i = 0; i < 6; i++) {
                DrawRectangle(275, 200 + (i * 55), 250, 45, GRAY);
            }
        } else {
            // Ve fázích 0 a 1 máme čisté pozadí
            ClearBackground(RAYWHITE);
            
            // Vykreslení loga s plynulou průhledností logoAlpha a efektem otřesu (shakeOffset)
            int currentY = 220 + (int)shakeOffset;
            int logoX = 260;
            
            // Velké modré logo TESCO (OPRAVENO NA ASSETMANAGER)
            DrawTextEx(AssetManager::mainFont, "TESCO", Vector2{(float)logoX, (float)currentY}, 80.0f, 2.0f, ColorAlpha(RED, logoAlpha));
            
            // BEZPEČNÉ PODTRŽENÍ: Kreslení velkých červených obdélníčků (velikost 80) s respektem k průhlednosti animace
            float letterWidth = 80 * 0.75f;  
            float dashWidth = letterWidth * 0.7f; 
            float dashHeight = 80 * 0.15f;  
            for (int i = 0; i < 5; i++) {
                int dashX = logoX + (i * letterWidth) + (letterWidth - dashWidth) / 2 - 20;
                int dashY = currentY + 80 + 2;
                DrawRectangle(dashX, dashY, (int)dashWidth, (int)dashHeight, ColorAlpha(BLUE, logoAlpha));
            }
        }

        // NA ZÁVĚR VŠEHO HODÍME ČERNOU OPONU (podle blackAlpha)
        if (blackAlpha > 0.0f) {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), ColorAlpha(BLACK, blackAlpha));
        }

}