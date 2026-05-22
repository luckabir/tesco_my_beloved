#include "scene_stats.h"
#include "../../managers/AssetManager.h"
#include "../../structures/Profile.h"
#include "raylib.h"
#include <cmath>

void runStatsScene(GameState &currentState, InputManager &input) {
    Vector2 mousePos = GetMousePosition();
    float scale = fminf((float)GetScreenWidth() / 800.0f, (float)GetScreenHeight() / 600.0f);
    mousePos.x = (mousePos.x - ((float)GetScreenWidth() - (800.0f * scale)) * 0.5f) / scale;
    mousePos.y = (mousePos.y - ((float)GetScreenHeight() - (600.0f * scale)) * 0.5f) / scale;

    ClearBackground(RAYWHITE);

    static bool isProgressSaved = false;

    // Save profile data only once when entering this state
    if (!isProgressSaved && isUserLoggedIn) {
        if (!currentShift.wasFired) {
            activeProfile.shiftsCompleted++; 
            activeProfile.maxScore += currentShift.moneyEarned;
            activeProfile.UpdateRank();
            SaveProfile();
        }
        isProgressSaved = true;
    }

    // Heading text based on results
    if (currentShift.wasFired) {
        DrawTextEx(AssetManager::mainFont, "TŘIKRÁT A DOST! DOSTAL JSI VYHAZOV!", Vector2{ 170, 60 }, 20.0f, 1.0f, RED);
    } else {
        DrawTextEx(AssetManager::mainFont, "DENNI REPORT BRIGADNIKA (UCTENKA)", Vector2{ 200, 60 }, 20.0f, 1.0f, BLACK);
    }

    // Stats card
    int cardY = 150;
    DrawRectangle(200, cardY, 400, 260, LIGHTGRAY);
    DrawRectangleLines(200, cardY, 400, 260, DARKGRAY);

    DrawTextEx(AssetManager::mainFont, TextFormat("Odpracovany den:    %d.", currentShift.currentDay), Vector2{ 230, (float)cardY + 30 }, 16.0f, 1.0f, BLACK);
    DrawTextEx(AssetManager::mainFont, TextFormat("Naskenovano zbozi:  %d ks", currentShift.itemsScanned), Vector2{ 230, (float)cardY + 70 }, 14.0f, 1.0f, BLACK);
    DrawTextEx(AssetManager::mainFont, TextFormat("Dopustil ses chyb:  %d / 3", currentShift.mistakesMade), Vector2{ 230, (float)cardY + 110 }, 14.0f, 1.0f, currentShift.mistakesMade >= 3 ? RED : BLACK);
    DrawTextEx(AssetManager::mainFont, TextFormat("Dnesni trzba kasy:  %d Kc", currentShift.moneyEarned), Vector2{ 230, (float)cardY + 170 }, 16.0f, 1.0f, GOLD);

    // Action button
    Rectangle actionBtn = { 250, 450, 300, 45 };
    bool hoverAction = CheckCollisionPointRec(mousePos, actionBtn);
    DrawRectangleRec(actionBtn, hoverAction ? BLUE : DARKBLUE);
    
    std::string buttonLabel = currentShift.wasFired ? "ZPET DO MENU" : "POKRAČOVAT DO KALENDARRE";
    DrawTextEx(AssetManager::mainFont, buttonLabel.c_str(), Vector2{ actionBtn.x + 35, actionBtn.y + 14 }, 14.0f, 1.0f, WHITE);

    if (hoverAction && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        isProgressSaved = false; // Reset tracking flag for next time
        
        if (currentShift.wasFired) {
            currentState = STATE_MENU;       // Fired -> main menu
        } else {
            currentSubState = SUB_CALENDAR;  // Success -> back to sub-calendar
        }
    }
}