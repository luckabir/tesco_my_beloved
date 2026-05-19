#include "../main.h"                   
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h" 
#include "raylib.h"
#include "menu.h"
#include <cmath>

void runMenu(GameState &currentState, InputManager &input) {
    Rectangle startButton    = { 275, 200, 250, 45 };     
    Rectangle settingsButton = { 275, 255, 250, 45 };  
    Rectangle scoreButton    = { 275, 310, 250, 45 };
    Rectangle profileButton  = { 275, 365, 250, 45 };
    Rectangle savesButton    = { 275, 420, 250, 45 };
    Rectangle exitButton     = { 275, 475, 250, 45 };

    Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), Camera2D{ Vector2{0,0}, Vector2{0,0}, 0.0f, fminf((float)GetScreenWidth()/800.0f, (float)GetScreenHeight()/600.0f) });
    
    if (CheckCollisionPointRec(mousePos, startButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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
    if (CheckCollisionPointRec(mousePos, savesButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_SAVES; 
    }
    if (CheckCollisionPointRec(mousePos, exitButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_EXIT; 
    }

        ClearBackground(RAYWHITE);

        DrawBouncingTescoLogo(230, 50, 100);

        if (CheckCollisionPointRec(mousePos, startButton)) DrawRectangleRec(startButton, BLUE);
        else DrawRectangleRec(startButton, GRAY);
        DrawTextEx(AssetManager::mainFont, "START", Vector2{startButton.x + 100, startButton.y + 14}, 14.0f, 1.0f, WHITE);

        if (CheckCollisionPointRec(mousePos, settingsButton)) DrawRectangleRec(settingsButton, BLUE);
        else DrawRectangleRec(settingsButton, GRAY);
        DrawTextEx(AssetManager::mainFont, "NASTAVENI", Vector2{settingsButton.x + 85, settingsButton.y + 14}, 14.0f, 1.0f, WHITE);

        if (CheckCollisionPointRec(mousePos, scoreButton)) DrawRectangleRec(scoreButton, BLUE);
        else DrawRectangleRec(scoreButton, GRAY);
        DrawTextEx(AssetManager::mainFont, "ZEBRICEK", Vector2{scoreButton.x + 95, scoreButton.y + 14}, 14.0f, 1.0f, WHITE);

        if (CheckCollisionPointRec(mousePos, profileButton)) DrawRectangleRec(profileButton, BLUE);
        else DrawRectangleRec(profileButton, GRAY);
        DrawTextEx(AssetManager::mainFont, "PROFIL", Vector2{profileButton.x + 100, profileButton.y + 14}, 14.0f, 1.0f, WHITE);

        if (CheckCollisionPointRec(mousePos, savesButton)) DrawRectangleRec(savesButton, BLUE);
        else DrawRectangleRec(savesButton, GRAY);
        DrawTextEx(AssetManager::mainFont, "ULOZENE POZICE", Vector2{savesButton.x + 60, savesButton.y + 14}, 14.0f, 1.0f, WHITE);

        if (CheckCollisionPointRec(mousePos, exitButton)) DrawRectangleRec(exitButton, RED); 
        else DrawRectangleRec(exitButton, DARKGRAY);
        DrawTextEx(AssetManager::mainFont, "UKONCIT HRU", Vector2{exitButton.x + 75, exitButton.y + 14}, 14.0f, 1.0f, WHITE);
    
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