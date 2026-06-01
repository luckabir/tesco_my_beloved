#include "scene_controls.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h"
#include "raylib.h"
#include <cmath>

void runControls(GameState &currentState, InputManager &input)
{

    if (input.IsBackTriggered()) {
        currentState = STATE_MENU;
        return;
    }

    Vector2 mousePos = GetMousePosition();

    float scale = fminf(
        (float)GetScreenWidth() / 800.0f,
        (float)GetScreenHeight() / 600.0f
    );

    mousePos.x = (mousePos.x - ((float)GetScreenWidth() - (800.0f * scale)) * 0.5f) / scale;
    mousePos.y = (mousePos.y - ((float)GetScreenHeight() - (600.0f * scale)) * 0.5f) / scale;

    ClearBackground(RAYWHITE);

    Color lightBlue = Color{ 90, 170, 255, 255 };
    Color lightRed = Color{ 255, 120, 120, 255 };

    DrawTextEx(AssetManager::mainFont,"OVLADANI POKLADNY",Vector2{ 190, 50 },24.0f,1.0f,BLACK);

    DrawRectangle(120, 115, 560, 340, LIGHTGRAY);
    DrawRectangleLines(120, 115, 560, 340, DARKGRAY);


    DrawTextEx(AssetManager::mainFont, "LEVA RUKA", Vector2{ 160, 145 }, 17.0f, 1.0f, BLUE);

    DrawTextEx(AssetManager::mainFont, "Pohyb:", Vector2{ 180, 175 }, 14.0f, 1.0f, lightBlue);
    DrawTextEx(AssetManager::mainFont, "W A S D", Vector2{ 245, 175 }, 14.0f, 1.0f, BLACK);

    DrawTextEx(AssetManager::mainFont, "Interakce:", Vector2{ 180, 205 }, 14.0f, 1.0f, lightBlue);
    DrawTextEx(AssetManager::mainFont, "E", Vector2{ 285, 205 }, 14.0f, 1.0f, BLACK);


    DrawTextEx(AssetManager::mainFont, "PRAVA RUKA", Vector2{ 160, 250 }, 17.0f, 1.0f, BLUE);

    DrawTextEx(AssetManager::mainFont, "Pohyb:", Vector2{ 180, 280 }, 14.0f, 1.0f, lightBlue);
    DrawTextEx(AssetManager::mainFont, "I J K L", Vector2{ 245, 280 }, 14.0f, 1.0f, BLACK);

    DrawTextEx(AssetManager::mainFont, "Interakce:", Vector2{ 180, 310 }, 14.0f, 1.0f, lightBlue);
    DrawTextEx(AssetManager::mainFont, "O", Vector2{ 285, 310 }, 14.0f, 1.0f, BLACK);


    DrawTextEx(AssetManager::mainFont, "R / T / Z / U", Vector2{ 450, 175 }, 14.0f, 1.0f, BLACK);
    DrawTextEx(AssetManager::mainFont, "QWERTZ", Vector2{ 570, 175 }, 14.0f, 1.0f, lightRed);

    Rectangle backBtn = { 250, 500, 300, 45 };
    bool hoverBack = CheckCollisionPointRec(mousePos, backBtn);

    DrawRectangleRec(backBtn, hoverBack ? BLUE : DARKBLUE);
    DrawTextEx(AssetManager::mainFont,"ZPET DO MENU",Vector2{ backBtn.x + 85, backBtn.y + 14 },14.0f,1.0f,WHITE);

    if (hoverBack && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_MENU;
    }
}