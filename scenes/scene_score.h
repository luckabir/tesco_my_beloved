#ifndef SCENE_SCORE_H
#define SCENE_SCORE_H

#include "../main.h"
#include "../managers/InputManager.h"
#include <string>

struct ScoreRow {
    std::string nickname;
    int bestShift;
    int bestStreak;
    int totalMoney;
};

void runScore(GameState &currentState, InputManager &input);

#endif