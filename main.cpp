#include "main.h"
#include "managers/InputManager.h"
#include "managers/AssetManager.h"
#include "structures/Settings.h"
#include "structures/Profile.h"
#include <string.h>
#include "scenes/scene_intro.h"
#include "scenes/game_main.h"
#include "scenes/scene_menu.h"
#include "scenes/scene_profile.h"
#include "scenes/scene_saves.h"
#include "scenes/scene_score.h"
#include "scenes/scene_settings.h"
#include <cmath>

int main() {
    const int virtualWidth = 800;
    const int virtualHeight = 600;
    const int targetFPS = 60;
    char title[100] = "JDEME NA BRIGADKU UWU";
    GameState currentState = STATE_INTRO;
    GameState lastState = STATE_MENU; 
    InputManager input;
    bool isGamePaused = false;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(virtualWidth, virtualHeight, title);
    SetTargetFPS(targetFPS);
    InitAudioDevice();
    
    LoadSettings();
    SetMasterVolume(gameSettings.volume);
    
    LoadProfilesList();
    AssetManager::LoadAll();
    
    PlayMusicStream(AssetManager::menuMusic);

    if (gameSettings.fullscreen) {
        ToggleFullscreen(); 
    }

    RenderTexture2D target = LoadRenderTexture(virtualWidth, virtualHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT); 
    
    while (!WindowShouldClose() && currentState != STATE_EXIT) {

        input.Update();


        if (currentState == STATE_INTRO || currentState == STATE_MENU || 
            currentState == STATE_SETTINGS || currentState == STATE_PROFILE || 
            currentState == STATE_SCORE || currentState == STATE_SAVES) 
        {
            SetMusicVolume(AssetManager::menuMusic, 1.0f);
            SetMusicVolume(AssetManager::gameMusic, 0.0f);
            UpdateMusicStream(AssetManager::menuMusic);
        }
        else if (currentState == STATE_PLAYING) 
        {
            if (isGamePaused) 
            {
                SetMusicVolume(AssetManager::menuMusic, 0.0f);
                SetMusicVolume(AssetManager::gameMusic, 0.0f);
            }
            else 
            {
                SetMusicVolume(AssetManager::menuMusic, 0.0f);
                SetMusicVolume(AssetManager::gameMusic, 1.0f);
                UpdateMusicStream(AssetManager::gameMusic);
            }
        }

        if (currentState != lastState) {
            switch (currentState) {
                case STATE_INTRO:
                    strcpy(title, "KEZ BY PRACE TAKY TAKHLE PRILETELA");
                    break;
                case STATE_MENU:
                    strcpy(title, "JDEME NA BRIGADKU UWU");
                    break;
                case STATE_PLAYING:
                    if (isUserLoggedIn) {
                        strcpy(title, "BOLEST ZACINA");
                    } else {
                        currentState = STATE_PROFILE;
                        strcpy(title, "NO MOMENT");
                    }
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
                runMenu(currentState, input, isGamePaused);
                break;
            case STATE_PLAYING:
                runGame(currentState, input, isGamePaused);
                break;
            case STATE_SETTINGS:
                runSettings(currentState, input);
                break;
            case STATE_SAVES:
                runSaves(currentState, input);
                break;
            case STATE_PROFILE:
                runProfile(currentState, input, isGamePaused);
                break;
            case STATE_SCORE:
                runScore(currentState, input);
                break;
            default:
                break;
        }

        if (currentState != STATE_INTRO) {
            std::string statusText = isUserLoggedIn ? "Pokladni: " + activeProfile.nickname : "Neprihlasen";
            Color textColor = isUserLoggedIn ? GREEN : RED;

            Vector2 textSize = MeasureTextEx(AssetManager::mainFont, statusText.c_str(), 14.0f, 1.0f);
            float textX = (float)virtualWidth - textSize.x - 20.0f;
            float textY = 20.0f;
            
            DrawTextEx(AssetManager::mainFont, statusText.c_str(), Vector2{ textX, textY }, 14.0f, 1.0f, textColor);
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