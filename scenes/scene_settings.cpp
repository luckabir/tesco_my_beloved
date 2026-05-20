#include "scene_settings.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h"
#include "../structures/Settings.h"
#include "raylib.h"
#include <cmath>

void runSettings(GameState& currentState, InputManager& input)
{
    // Reference na globální settings
    float& volume = gameSettings.volume;
    bool& isFullscreen = gameSettings.fullscreen;

    Rectangle screenModeButton = { 250, 200, 300, 45 };
    Rectangle volumeSliderBar  = { 250, 320, 300, 15 };
    Rectangle backButton       = { 275, 450, 250, 45 };

    Vector2 mousePos = GetMousePosition();

    // Přepočet myši na canvas
    float scale = fminf((float)GetScreenWidth() / 800.0f, (float)GetScreenHeight() / 600.0f);
    mousePos.x = (mousePos.x - ((float)GetScreenWidth() - (800.0f * scale)) * 0.5f) / scale;
    mousePos.y = (mousePos.y - ((float)GetScreenHeight() - (600.0f * scale)) * 0.5f) / scale;

    // ===== INPUT =====

    // Spatřen stisk tlačítka Myši nebo Klávesy pro návrat
    bool backPressed = (CheckCollisionPointRec(mousePos, backButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT));
    
    if (input.IsBackTriggered() || backPressed)
    {
        // 1. OPRAVA: Uložíme to na disk JEN JEDNOU při odchodu ze scény
        SaveSettings(); 
        currentState = STATE_MENU;
        return;
    }

    // FULLSCREEN
    if (CheckCollisionPointRec(mousePos, screenModeButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        isFullscreen = !isFullscreen;
        ToggleFullscreen();
        // Zde klidně uložení nech, to se neděje 60x za vteřinu
        SaveSettings(); 
    }

    // SLIDER
    if (CheckCollisionPointRec(mousePos, volumeSliderBar) && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        volume = (mousePos.x - volumeSliderBar.x) / volumeSliderBar.width;

        if (volume < 0.0f) volume = 0.0f;
        if (volume > 1.0f) volume = 1.0f;

        // Měníme hlasitost v reálném čase v paměti
        SetMasterVolume(volume);
        
        // !!! ODSTRANĚNO: SaveSettings() – už netrápíme disk při každém pohnutí myši
    }

    // ===== DRAW =====
    // (Zde tvůj vykreslovací kód pokračuje naprosto beze změn...)
    DrawTextEx(AssetManager::mainFont, "NASTAVENI", Vector2{ 310, 80 }, 30.0f, 2.0f, BLACK);

    // SCREEN MODE
    DrawTextEx(AssetManager::mainFont, "REZIM OBRAZOVKY:", Vector2{ 250, 170 }, 16.0f, 1.0f, DARKGRAY);
    DrawRectangleRec(screenModeButton, CheckCollisionPointRec(mousePos, screenModeButton) ? LIGHTGRAY : GRAY);

    if (isFullscreen) {
        DrawTextEx(AssetManager::mainFont, "CELA OBRAZOVKA", Vector2{ screenModeButton.x + 45, screenModeButton.y + 14 }, 14.0f, 1.0f, BLACK);
    } else {
        DrawTextEx(AssetManager::mainFont, "V OKNE", Vector2{ screenModeButton.x + 95, screenModeButton.y + 14 }, 14.0f, 1.0f, BLACK);
    }

    // VOLUME
    DrawTextEx(AssetManager::mainFont, "HLASITOST:", Vector2{ 250, 290 }, 16.0f, 1.0f, DARKGRAY);
    DrawRectangleRec(volumeSliderBar, LIGHTGRAY);
    DrawRectangle((int)volumeSliderBar.x, (int)volumeSliderBar.y, (int)(volumeSliderBar.width * volume), (int)volumeSliderBar.height, RED);
    DrawRectangle((int)(volumeSliderBar.x + volumeSliderBar.width * volume - 8), (int)volumeSliderBar.y - 5, 16, 25, DARKGRAY);

    char volText[16];
    TextCopy(volText, TextFormat("%d %%", (int)(volume * 100)));
    DrawTextEx(AssetManager::mainFont, volText, Vector2{ volumeSliderBar.x + volumeSliderBar.width + 20, volumeSliderBar.y - 2 }, 16.0f, 1.0f, BLACK);

    // BACK BUTTON
    DrawRectangleRec(backButton, CheckCollisionPointRec(mousePos, backButton) ? LIGHTGRAY : GRAY);
    DrawTextEx(AssetManager::mainFont, "ZPET DO MENU", Vector2{ backButton.x + 45, backButton.y + 14 }, 14.0f, 1.0f, BLACK);
}