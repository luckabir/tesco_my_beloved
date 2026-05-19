#include "main.h"
#include "managers/InputManager.h"
#include "managers/AssetManager.h"
#include "structures/Settings.h"
#include <string.h>
#include <cmath>

int main() {
    const int virtualWidth = 800;
    const int virtualHeight = 600;
    const int targetFPS = 60;
    char title[100] = "JDEME NA BRIGADKU UWU";
    GameState currentState = STATE_INTRO;
    GameState lastState = STATE_MENU; 
    InputManager input;
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(virtualWidth, virtualHeight, title);
    SetTargetFPS(targetFPS);
    InitAudioDevice();
    LoadSettings();
    AssetManager::LoadAll();
    PlayMusicStream(AssetManager::bgMusic);
    SetMusicVolume(AssetManager::bgMusic, gameSettings.volume);

    if (gameSettings.fullscreen) {
        ToggleFullscreen(); // <--- OPRAVA FULLSCREENU PO STARTU
    }

    RenderTexture2D target = LoadRenderTexture(virtualWidth, virtualHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT); 
    
    while (!WindowShouldClose() && currentState != STATE_EXIT) {

        UpdateMusicStream(AssetManager::bgMusic);
        input.Update();

        if (currentState != lastState) {
            switch (currentState) {
                case STATE_INTRO:
                    strcpy(title, "KEZ BY PRACE TAKY TAKHLE PRILETELA");
                    break;
                case STATE_MENU:
                    strcpy(title, "JDEME NA BRIGADKU UWU");
                    break;
                case STATE_PLAYING:
                    strcpy(title, "BOLEST ZACINA");
                    break;
                case STATE_SETTINGS:
                    strcpy(title, "TADY JE SNAD NECO SPATNE OTAZNIK");
                    break;
                case STATE_SCORE:
                    strcpy(title, "JSI TU MORE");
                    break;
                case STATE_PROFILE:
                    strcpy(title, "ANONYMNI REZIM");
                    break;
                case STATE_SAVES:
                    strcpy(title, "TOHLE JE NA TOM LIP JAK MOJE STUDIUM");
                    break;
            }
            SetWindowTitle(title);
            lastState = currentState; 
        }

        BeginTextureMode(target);
        ClearBackground(RAYWHITE);
        switch (currentState) {
            case STATE_INTRO:
                runIntro(currentState, input);
                break;
            case STATE_MENU:
                runMenu(currentState, input);
                break;
            case STATE_PLAYING:
                runGame(currentState, input);
                break;
            case STATE_SETTINGS:
                runSettings(currentState, input);
                break;
            case STATE_SAVES:
                runSaves(currentState, input);
                break;
            case STATE_PROFILE:
                runProfile(currentState, input);
                break;
            case STATE_SCORE:
                runScore(currentState, input);
                break;
            default:
                break;
        }
        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK); 

            float scale = fminf((float)GetScreenWidth() / virtualWidth, (float)GetScreenHeight() / virtualHeight);

            DrawTexturePro(
                target.texture,
                Rectangle{ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                Rectangle{ 
                    ((float)GetScreenWidth() - ((float)virtualWidth * scale)) * 0.5f,
                    ((float)GetScreenHeight() - ((float)virtualHeight * scale)) * 0.5f,
                    (float)virtualWidth * scale,
                    (float)virtualHeight * scale
                },
                Vector2{ 0, 0 },
                0.0f,
                WHITE
            );
        EndDrawing();
    }

    UnloadRenderTexture(target);
    AssetManager::UnloadAll();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}