#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "../main.h"
#include "../managers/InputManager.h"

struct ShiftData {
    int currentDay = 1;       
    int moneyEarned = 0;     
    int itemsScanned = 0;    
    int mistakesMade = 0;   
    bool wasFired = false;
};

enum GameSubState {
    SUB_CALENDAR,
    SUB_PLAYING_CASHIER,
    SUB_STATS
};

extern ShiftData currentShift;
extern bool resetGameSignal;
extern GameSubState currentSubState;

void runGame(GameState &currentState, InputManager &input, bool& isGamePaused);

#endif