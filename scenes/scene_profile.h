#ifndef SCENE_PROFILE_H
#define SCENE_PROFILE_H

#include "../main.h"

enum ProfileSubState {
    SUB_SEZNAM,
    SUB_DETAIL,
    SUB_EDITACE
};

void runProfile(GameState &currentState, InputManager &input, bool& isGamePaused);

#endif