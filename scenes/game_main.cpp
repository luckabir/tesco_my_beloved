#include "game_main.h"
#include "game/scene_calendar.h"
#include "game/scene_game.h"
#include "game/scene_stats.h"

ShiftData currentShift;
bool resetGameSignal = false;
GameSubState currentSubState = SUB_CALENDAR;

void runGame(GameState &currentState, InputManager &input, bool &isGamePaused) {
    switch (currentSubState) {
        case SUB_CALENDAR:
            runCalendarScene(currentState, input);
            break;

        case SUB_PLAYING_CASHIER:
            runGameRecieved(currentState, input, isGamePaused, currentSubState);
            break;

        case SUB_STATS:
            runStatsScene(currentState, input);
            break;
    }
}