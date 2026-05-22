#include "game_main.h"
#include "game/scene_calendar.h"
#include "game/scene_game.h"
#include "game/scene_stats.h"

// Global tracking variable for the sub-state
ShiftData currentShift;
bool resetGameSignal = false;
GameSubState currentSubState = SUB_CALENDAR;

void runGame(GameState &currentState, InputManager &input, bool &isGamePaused) {
    // If we receive a reset signal from the main menu / profile
    if (resetGameSignal) {
        currentSubState = SUB_CALENDAR;
        // Notice: resetGameSignal is left true here, 
        // because scene_game.cpp needs it to reset its own static variables.
    }

    // Sub-state switcher passing standard parameters
    switch (currentSubState) {
        case SUB_CALENDAR:
            runCalendarScene(currentState, input);
            break;

        case SUB_PLAYING_CASHIER:
            // This is your actual cashier gameplay function inside scene_game.cpp
            runGameRecieved(currentState, input, isGamePaused, currentSubState);
            break;

        case SUB_STATS:
            runStatsScene(currentState, input);
            break;
    }
}