#include "scene_calendar.h"
#include "../../managers/AssetManager.h"
#include "../../structures/Profile.h"
#include "raylib.h"
#include <cmath>

void runCalendarScene(GameState &currentState, InputManager &input) {
    Vector2 mousePos = GetMousePosition();
    float scale = fminf((float)GetScreenWidth() / 800.0f, (float)GetScreenHeight() / 600.0f);
    mousePos.x = (mousePos.x - ((float)GetScreenWidth() - (800.0f * scale)) * 0.5f) / scale;
    mousePos.y = (mousePos.y - ((float)GetScreenHeight() - (600.0f * scale)) * 0.5f) / scale;

    ClearBackground(RAYWHITE);

    // Sync current day with the player profile
    if (isUserLoggedIn) {
        currentShift.currentDay = activeProfile.shiftsCompleted + 1;
    }

    // Drawing titles
    DrawTextEx(AssetManager::mainFont, "PLAN SMEN - TESCO KARIERA", Vector2{ 220, 60 }, 22.0f, 1.0f, BLACK);
    
    // Info box
    DrawRectangle(200, 140, 400, 100, LIGHTGRAY);
    DrawRectangleLines(200, 140, 400, 100, DARKGRAY);
    DrawTextEx(AssetManager::mainFont, TextFormat("Aktualni den: %d. sichta", currentShift.currentDay), Vector2{ 230, 160 }, 16.0f, 1.0f, BLACK);
    DrawTextEx(AssetManager::mainFont, TextFormat("Celkem odpracovano: %d dni", activeProfile.shiftsCompleted), Vector2{ 230, 195 }, 14.0f, 1.0f, DARKGRAY);

    // Start Button
    Rectangle startShiftBtn = { 250, 300, 300, 50 };
    bool hoverStart = CheckCollisionPointRec(mousePos, startShiftBtn);
    DrawRectangleRec(startShiftBtn, hoverStart ? LIME : GREEN);
    DrawTextEx(AssetManager::mainFont, "NASTOUPIT NA SMENU", Vector2{ startShiftBtn.x + 50, startShiftBtn.y + 16 }, 14.0f, 1.0f, WHITE);

    // Back Button
    Rectangle backToMenuBtn = { 250, 380, 300, 40 };
    bool hoverBack = CheckCollisionPointRec(mousePos, backToMenuBtn);
    DrawRectangleRec(backToMenuBtn, hoverBack ? LIGHTGRAY : GRAY);
    DrawTextEx(AssetManager::mainFont, "NAVRAT DO MENU", Vector2{ backToMenuBtn.x + 75, backToMenuBtn.y + 12 }, 14.0f, 1.0f, WHITE);

    // Input processing
    if (hoverStart && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // Reset counters for the new shift
        currentShift.moneyEarned = 0;
        currentShift.itemsScanned = 0;
        currentShift.mistakesMade = 0;
        currentShift.wasFired = false;
        
        resetGameSignal = true;          // Let scene_game.cpp know it should re-initialize
        currentSubState = SUB_PLAYING_CASHIER; // Switch sub-state directly
    }

    if (hoverBack && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_MENU;       // Return to main menu via main state machine
    }
}