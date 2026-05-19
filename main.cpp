#include "main.h"
#include "managers/InputManager.h"
#include "managers/AssetManager.h"
#include <string.h>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    const int targetFPS = 60;
    char title[100] = "JDEME NA BRIGADKU UWU";
    GameState currentState = STATE_INTRO;
    GameState lastState = STATE_MENU; 
    InputManager input;
    
    InitWindow(screenWidth, screenHeight, title);
    SetTargetFPS(targetFPS);
    InitAudioDevice();

    AssetManager::LoadAll();
    PlayMusicStream(AssetManager::bgMusic);
    SetMusicVolume(AssetManager::bgMusic, 0.5f);


    while (!WindowShouldClose() && currentState != STATE_EXIT) {

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
    }

    AssetManager::UnloadAll();
    CloseAudioDevice();
    CloseWindow();
    return 0;
}