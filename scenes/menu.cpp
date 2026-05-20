#include "../main.h"                   
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h" 
#include "../structures/Profile.h" // Potřebujeme pro isUserLoggedIn a activeProfile
#include "raylib.h"
#include "menu.h"
#include <cmath>

bool HasSaveGame() 
{
    if (!isUserLoggedIn) return false;
    
    if (activeProfile.shiftsCompleted > 0 || activeProfile.maxScore > 0) { 
        return true; 
    }
    return false;
}

void runMenu(GameState &currentState, InputManager &input, bool& isGamePaused) {
    // --- PŘESUNUTÉ POZICE TLAČÍTEK (Posunuté dolů o cca 50px kvůli logu) ---
    Rectangle startButton    = { 275, 170, 250, 40 };     
    Rectangle continueButton = { 275, 220, 250, 40 }; // <--- OPRAVA: Přidán chybějící obdélník
    Rectangle settingsButton = { 275, 270, 250, 40 };  
    Rectangle scoreButton    = { 275, 320, 250, 40 };
    Rectangle profileButton  = { 275, 370, 250, 40 };
    Rectangle savesButton    = { 275, 420, 250, 40 };
    Rectangle exitButton     = { 275, 470, 250, 40 };

    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), Camera2D{ Vector2{0,0}, Vector2{0,0}, 0.0f, fminf((float)GetScreenWidth()/800.0f, (float)GetScreenHeight()/600.0f) });
    
    bool canContinue = isGamePaused || HasSaveGame(); 

    // ===== LOGIKA INPUTŮ =====

    // 1. START
    if (CheckCollisionPointRec(mousePos, startButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (isUserLoggedIn) {
            isGamePaused = false; 
            currentState = STATE_PLAYING; 
        } else {
            currentState = STATE_PROFILE; 
        }
    }

    // 2. POKRAČOVAT
    if (canContinue && CheckCollisionPointRec(mousePos, continueButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (isGamePaused) {
            isGamePaused = false;
        } else {
            // Zde případně tvoje LoadLatestSave();
        }
        currentState = STATE_PLAYING; 
    }

    // OSTATNÍ TLAČÍTKA
    if (CheckCollisionPointRec(mousePos, settingsButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_SETTINGS; 
    }
    if (CheckCollisionPointRec(mousePos, scoreButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_SCORE; 
    }
    if (CheckCollisionPointRec(mousePos, profileButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_PROFILE; 
    }
    if (CheckCollisionPointRec(mousePos, savesButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_SAVES; 
    }
    if (CheckCollisionPointRec(mousePos, exitButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_EXIT; 
    }

    // ===== VYKRESLOVÁNÍ =====
    ClearBackground(RAYWHITE);

    // Skákající TESCO logo (startY posunuto na 30, ať netlačí na tlačítka)
    DrawBouncingTescoLogo(230, 30, 90);

    // 1. START / NOVÁ BRIGÁDA
    if (CheckCollisionPointRec(mousePos, startButton)) DrawRectangleRec(startButton, BLUE);
    else DrawRectangleRec(startButton, GRAY);
    DrawTextEx(AssetManager::mainFont, "NOVA BRIGADA", Vector2{startButton.x + 65, startButton.y + 12}, 14.0f, 1.0f, WHITE);

    // 2. POKRAČOVAT VE HŘE (Podmínka pro zašednutí / aktivaci)
    if (canContinue) {
        if (CheckCollisionPointRec(mousePos, continueButton)) DrawRectangleRec(continueButton, BLUE);
        else DrawRectangleRec(continueButton, GRAY);
        DrawTextEx(AssetManager::mainFont, "POKRACOVAT VE HRE", Vector2{continueButton.x + 45, continueButton.y + 12}, 14.0f, 1.0f, WHITE);
    } else {
        DrawRectangleRec(continueButton, LIGHTGRAY); // Zašedlé
        DrawTextEx(AssetManager::mainFont, "POKRACOVAT VE HRE", Vector2{continueButton.x + 45, continueButton.y + 12}, 14.0f, 1.0f, DARKGRAY);
    }

    // 3. NASTAVENÍ
    if (CheckCollisionPointRec(mousePos, settingsButton)) DrawRectangleRec(settingsButton, BLUE);
    else DrawRectangleRec(settingsButton, GRAY);
    DrawTextEx(AssetManager::mainFont, "NASTAVENI", Vector2{settingsButton.x + 85, settingsButton.y + 12}, 14.0f, 1.0f, WHITE);

    // 4. ŽEBŘÍČEK
    if (CheckCollisionPointRec(mousePos, scoreButton)) DrawRectangleRec(scoreButton, BLUE);
    else DrawRectangleRec(scoreButton, GRAY);
    DrawTextEx(AssetManager::mainFont, "ZEBRICEK", Vector2{scoreButton.x + 95, scoreButton.y + 12}, 14.0f, 1.0f, WHITE);

    // 5. PROFIL
    if (CheckCollisionPointRec(mousePos, profileButton)) DrawRectangleRec(profileButton, BLUE);
    else DrawRectangleRec(profileButton, GRAY);
    DrawTextEx(AssetManager::mainFont, "PROFIL", Vector2{profileButton.x + 100, profileButton.y + 12}, 14.0f, 1.0f, WHITE);

    // 6. ULOŽENÉ POZICE
    if (CheckCollisionPointRec(mousePos, savesButton)) DrawRectangleRec(savesButton, BLUE);
    else DrawRectangleRec(savesButton, GRAY);
    DrawTextEx(AssetManager::mainFont, "ULOZENE POZICE", Vector2{savesButton.x + 60, savesButton.y + 12}, 14.0f, 1.0f, WHITE);

    // 7. UKONČIT HRU
    if (CheckCollisionPointRec(mousePos, exitButton)) DrawRectangleRec(exitButton, RED); 
    else DrawRectangleRec(exitButton, DARKGRAY);
    DrawTextEx(AssetManager::mainFont, "UKONCIT HRU", Vector2{exitButton.x + 75, exitButton.y + 12}, 14.0f, 1.0f, WHITE);
}

void DrawBouncingTescoLogo(int startX, int startY, int logoSize) {
    // --- NASTAVENÍ BOUNCE EFEKTU ---
    float bounceOffset = sinf(GetTime() * 3.0f) * 6.0f; 

    // Připočteme bounceOffset k původní Y pozici
    float currentY = (float)startY + bounceOffset;

    // Vykreslení textu TESCO přes globální AssetManager::mainFont (Červeně)
    DrawTextEx(AssetManager::mainFont, "TESCO", Vector2{(float)startX, currentY}, (float)logoSize, 2.0f, RED);
                
    // Vykreslení rozkouskovaného podtržení (Modře)
    float menuLetterWidth = logoSize * 0.75f;  
    float menuDashWidth = menuLetterWidth * 0.7f; 
    float menuDashHeight = logoSize * 0.15f;  

    for (int i = 0; i < 5; i++) {
        int menuDashX = startX + (i * menuLetterWidth) + (menuLetterWidth - menuDashWidth) / 2 - 20;
        int menuDashY = (int)currentY + logoSize + 2;
        
        DrawRectangle(menuDashX, menuDashY, (int)menuDashWidth, (int)menuDashHeight, BLUE);
    }
}