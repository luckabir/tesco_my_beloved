#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"

class InputManager;

enum GameState {
    STATE_INTRO,
    STATE_MENU,
    STATE_PLAYING,
    STATE_SETTINGS,
    STATE_SCORE,
    STATE_PROFILE,
    STATE_SAVES,
    STATE_EXIT,
};

void runIntro(GameState &currentState, InputManager &input, Font &myFont);
void runMenu(GameState &currentState, InputManager &input, Font &myFont);
void runGame(GameState &currentState, InputManager &input, Font &myFont);
void runSettings(GameState &currentState, InputManager &input, Font &myFont);
void runScore(GameState &currentState, InputManager &input, Font &myFont);
void runProfile(GameState &currentState, InputManager &input, Font &myFont);
void runSaves(GameState &currentState, InputManager &input, Font &myFont);

#endif