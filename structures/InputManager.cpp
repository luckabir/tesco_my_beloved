#include "InputManager.h"

InputManager::InputManager() {
    keyUp    = KEY_W;
    keyDown  = KEY_S;
    keyLeft  = KEY_A;
    keyRight = KEY_D;
    keyBack  = KEY_BACKSPACE;
    keyPause = KEY_SPACE;
}

void InputManager::Update() {
    actionMoveUp    = IsKeyDown(keyUp)    || IsKeyDown(KEY_UP);    
    actionMoveDown  = IsKeyDown(keyDown)  || IsKeyDown(KEY_DOWN);
    actionMoveLeft  = IsKeyDown(keyLeft)  || IsKeyDown(KEY_LEFT);
    actionMoveRight = IsKeyDown(keyRight) || IsKeyDown(KEY_RIGHT);
    actionBack      = IsKeyPressed(keyBack) || IsKeyPressed(KEY_ESCAPE);
    actionPause     = IsKeyPressed(keyPause);
}