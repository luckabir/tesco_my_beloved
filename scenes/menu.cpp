#include "../main.h"                   
#include "../structures/InputManager.h" 
#include "raylib.h"
#include "menu.h"
#include <cmath>

void runMenu(GameState &currentState, InputManager &input, Font &myFont) {
    Rectangle startButton    = { 275, 200, 250, 45 };     
    Rectangle settingsButton = { 275, 255, 250, 45 };  
    Rectangle scoreButton    = { 275, 310, 250, 45 };
    Rectangle profileButton  = { 275, 365, 250, 45 };
    Rectangle savesButton    = { 275, 420, 250, 45 };
    Rectangle exitButton     = { 275, 475, 250, 45 };

    Vector2 mousePos = GetMousePosition();

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

    BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawBouncingTescoLogo(myFont, 230, 50, 100);

        if (CheckCollisionPointRec(mousePos, startButton)) DrawRectangleRec(startButton, LIGHTGRAY);
        else DrawRectangleRec(startButton, GRAY);
        DrawText("START", startButton.x + 95, startButton.y + 12, 20, BLACK);

        if (CheckCollisionPointRec(mousePos, settingsButton)) DrawRectangleRec(settingsButton, LIGHTGRAY);
        else DrawRectangleRec(settingsButton, GRAY);
        DrawText("NASTAVENI", settingsButton.x + 75, settingsButton.y + 12, 20, BLACK);

        if (CheckCollisionPointRec(mousePos, scoreButton)) DrawRectangleRec(scoreButton, LIGHTGRAY);
        else DrawRectangleRec(scoreButton, GRAY);
        DrawText("ZEBRICEK", scoreButton.x + 80, scoreButton.y + 12, 20, BLACK);

        if (CheckCollisionPointRec(mousePos, profileButton)) DrawRectangleRec(profileButton, LIGHTGRAY);
        else DrawRectangleRec(profileButton, GRAY);
        DrawText("PROFIL", profileButton.x + 90, profileButton.y + 12, 20, BLACK);

        if (CheckCollisionPointRec(mousePos, savesButton)) DrawRectangleRec(savesButton, LIGHTGRAY);
        else DrawRectangleRec(savesButton, GRAY);
        DrawText("ULOZENE POZICE", savesButton.x + 50, savesButton.y + 12, 20, BLACK);

        if (CheckCollisionPointRec(mousePos, exitButton)) DrawRectangleRec(exitButton, RED); // Ukončit dáme červeně, když na něj najedeš
        else DrawRectangleRec(exitButton, DARKGRAY);
        DrawText("UKONCIT HRU", exitButton.x + 65, exitButton.y + 12, 20, WHITE);

    EndDrawing();
}



// Funkce, která vykreslí skákající Tesco logo s podtržením <3
void DrawBouncingTescoLogo(Font &myFont, int startX, int startY, int logoSize) {
    // --- NASTAVENÍ BOUNCE EFEKTU ---
    // GetTime() vrací čas v sekundách od spuštění hry.
    // Násobení u času (např. 3.0f) určuje RYCHLOST skákání.
    // Násobení celého sinu (např. 6.0f) určuje VÝŠKU skoku (jak moc to skáče nahoru/dolu).
    float bounceOffset = sinf(GetTime() * 3.0f) * 6.0f; 

    // Připočteme bounceOffset k původní Y pozici
    float currentY = (float)startY + bounceOffset;

    // 1. Vykreslení textu TESCO (Červeně)
    DrawTextEx(myFont, "TESCO", Vector2{(float)startX, currentY}, (float)logoSize, 2.0f, RED);
                
    // 2. Vykreslení rozkouskovaného podtržení (Modře)
    float menuLetterWidth = logoSize * 0.75f;  
    float menuDashWidth = menuLetterWidth * 0.7f; 
    float menuDashHeight = logoSize * 0.15f;  

    for (int i = 0; i < 5; i++) {
        // Čárky se posouvají na Y společně s textem, protože berou "currentY"
        int menuDashX = startX + (i * menuLetterWidth) + (menuLetterWidth - menuDashWidth) / 2 - 20;
        int menuDashY = (int)currentY + logoSize + 2;
        
        DrawRectangle(menuDashX, menuDashY, (int)menuDashWidth, (int)menuDashHeight, BLUE);
    }
}