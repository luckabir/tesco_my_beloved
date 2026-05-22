#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "../main.h"
#include "../managers/InputManager.h"

struct ShiftData {
    int currentDay = 1;       // Aktuální den na brigádě
    int moneyEarned = 0;      // Peníze utržené v daném dni
    int itemsScanned = 0;     // Počet naskenovaných položek
    int mistakesMade = 0;     // Počet chyb
    bool wasFired = false;    // Pokud hráč udělá příliš chyb, dostane vyhazov
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