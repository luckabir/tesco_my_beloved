#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "raylib.h"

class InputManager {
private:
    int keyUp;
    int keyDown;
    int keyLeft;
    int keyRight;
    int keyBack;
    int keyPause;

    bool actionMoveUp;
    bool actionMoveDown;
    bool actionMoveLeft;
    bool actionMoveRight;
    bool actionBack;
    bool actionPause;

public:
    InputManager(); 

    void Update();  

    bool IsMovingUp() const    { return actionMoveUp; }
    bool IsMovingDown() const  { return actionMoveDown; }
    bool IsMovingLeft() const  { return actionMoveLeft; }
    bool IsMovingRight() const { return actionMoveRight; }
    bool IsBackTriggered() const { return actionBack; }
    bool IsPauseTriggered() const { return actionPause; }

    void RemapUp(int newKey) { keyUp = newKey; }
};

#endif