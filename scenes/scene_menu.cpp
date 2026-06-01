#include "../main.h"  
#include "game_main.h"                 
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h" 
#include "../structures/Profile.h" 
#include "raylib.h"
#include "scene_menu.h"
#include <cmath>

extern ShiftData currentShift;
extern bool resetGameSignal;
extern GameSubState currentSubState;

void runMenu(GameState &currentState, InputManager &input, bool& isGamePaused) {
    Rectangle startButton    = { 275, 170, 250, 40 };     
    Rectangle continueButton = { 275, 220, 250, 40 };
    Rectangle settingsButton = { 275, 270, 250, 40 };  
    Rectangle scoreButton    = { 275, 320, 250, 40 };
    Rectangle profileButton  = { 275, 370, 250, 40 };
    Rectangle controlsButton  = { 275, 420, 250, 40 };
    Rectangle exitButton     = { 275, 470, 250, 40 };

    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), Camera2D{ Vector2{0,0}, Vector2{0,0}, 0.0f, fminf((float)GetScreenWidth()/800.0f, (float)GetScreenHeight()/600.0f) });
    
    float scale = fminf(
        (float)GetScreenWidth() / 800.0f,
        (float)GetScreenHeight() / 600.0f
    );

    mousePos.x = (mousePos.x - ((float)GetScreenWidth() - (800.0f * scale)) * 0.5f) / scale;
    mousePos.y = (mousePos.y - ((float)GetScreenHeight() - (600.0f * scale)) * 0.5f) / scale;

    bool canContinue = isUserLoggedIn && (isGamePaused || HasSaveGame());

    if (CheckCollisionPointRec(mousePos, startButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (isUserLoggedIn) {
            isGamePaused = false; 
            resetGameSignal = true;
            currentSubState = SUB_CALENDAR;
            activeProfile.shiftsCompleted = 0;
            activeProfile.totalMoneyEarned = 0;
            activeProfile.customersServed = 0;
            activeProfile.shiftsCompleted = 0;
            activeProfile.totalMoneyEarned = 0;
            activeProfile.customersServed = 0;
            SaveProfile();
            currentState = STATE_PLAYING; 
        } else {
            currentState = STATE_PROFILE; 
        }
    }

    if (canContinue && CheckCollisionPointRec(mousePos, continueButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (isGamePaused) {
            isGamePaused = false;
        }
        currentState = STATE_PLAYING; 
    }

    if (CheckCollisionPointRec(mousePos, settingsButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_SETTINGS; 
    }
    if (CheckCollisionPointRec(mousePos, scoreButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_SCORE; 
    }
    if (CheckCollisionPointRec(mousePos, profileButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_PROFILE; 
    }

    if (CheckCollisionPointRec(mousePos, exitButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_EXIT; 
    }

    if (CheckCollisionPointRec(mousePos, controlsButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_CONTROLS;
    }

    ClearBackground(RAYWHITE);

    DrawBouncingTescoLogo(250, 30, 90);

    if (CheckCollisionPointRec(mousePos, startButton)) DrawRectangleRec(startButton, BLUE);
    else DrawRectangleRec(startButton, GRAY);
    DrawTextEx(AssetManager::mainFont, "NOVA BRIGADA", Vector2{startButton.x + 65, startButton.y + 12}, 14.0f, 1.0f, WHITE);

    if (canContinue) {
        if (CheckCollisionPointRec(mousePos, continueButton)) DrawRectangleRec(continueButton, BLUE);
        else DrawRectangleRec(continueButton, GRAY);
        DrawTextEx(AssetManager::mainFont, "POKRACOVAT VE HRE", Vector2{continueButton.x + 45, continueButton.y + 12}, 14.0f, 1.0f, WHITE);
    } else {
        DrawRectangleRec(continueButton, LIGHTGRAY); // Zašedlé
        DrawTextEx(AssetManager::mainFont, "POKRACOVAT VE HRE", Vector2{continueButton.x + 45, continueButton.y + 12}, 14.0f, 1.0f, DARKGRAY);
    }

    if (CheckCollisionPointRec(mousePos, settingsButton)) DrawRectangleRec(settingsButton, BLUE);
    else DrawRectangleRec(settingsButton, GRAY);
    DrawTextEx(AssetManager::mainFont, "NASTAVENI", Vector2{settingsButton.x + 85, settingsButton.y + 12}, 14.0f, 1.0f, WHITE);

    if (CheckCollisionPointRec(mousePos, scoreButton)) DrawRectangleRec(scoreButton, BLUE);
    else DrawRectangleRec(scoreButton, GRAY);
    DrawTextEx(AssetManager::mainFont, "ZEBRICEK", Vector2{scoreButton.x + 95, scoreButton.y + 12}, 14.0f, 1.0f, WHITE);

    if (CheckCollisionPointRec(mousePos, profileButton)) DrawRectangleRec(profileButton, BLUE);
    else DrawRectangleRec(profileButton, GRAY);
    DrawTextEx(AssetManager::mainFont, "PROFIL", Vector2{profileButton.x + 100, profileButton.y + 12}, 14.0f, 1.0f, WHITE);

    if (CheckCollisionPointRec(mousePos, controlsButton)) DrawRectangleRec(controlsButton, BLUE); 
    else DrawRectangleRec(controlsButton, GRAY);
    DrawTextEx(AssetManager::mainFont, "OVLADANI", Vector2{controlsButton.x + 95, controlsButton.y + 12}, 14.0f, 1.0f, WHITE);

    if (CheckCollisionPointRec(mousePos, exitButton)) DrawRectangleRec(exitButton, RED); 
    else DrawRectangleRec(exitButton, DARKGRAY);
    DrawTextEx(AssetManager::mainFont, "UKONCIT HRU", Vector2{exitButton.x + 75, exitButton.y + 12}, 14.0f, 1.0f, WHITE);
}


bool HasSaveGame() 
{
    if (!isUserLoggedIn) return false;
    
    if (activeProfile.shiftsCompleted > 0) { 
        return true; 
    }
    return false;
}

void DrawBouncingTescoLogo(int startX, int startY, int logoSize) {
    float bounceOffset = sinf(GetTime() * 3.0f) * 6.0f; 

    float currentY = (float)startY + bounceOffset;

    DrawTextEx(AssetManager::mainFont, "TESCO", Vector2{(float)startX, currentY}, (float)logoSize, 2.0f, RED);
                
    float menuLetterWidth = logoSize * 0.75f;  
    float menuDashWidth = menuLetterWidth * 0.7f; 
    float menuDashHeight = logoSize * 0.15f;  

    for (int i = 0; i < 5; i++) {
        int menuDashX = startX + (i * menuLetterWidth) + (menuLetterWidth - menuDashWidth) / 2 - 20;
        int menuDashY = (int)currentY + logoSize + 2;
        
        DrawRectangle(menuDashX, menuDashY, (int)menuDashWidth, (int)menuDashHeight, BLUE);
    }
}