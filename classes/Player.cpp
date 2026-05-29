#include "Player.h"

void Hand::Draw() const
{
    Vector2 elbow = isLeft ? Vector2{ 100, 700 } : Vector2{ 700, 700 };

    DrawLineEx(elbow, pos, 35.0f, skinColor);
    DrawCircleV(pos, 25, skinColor);

    DrawCircle((int)pos.x, (int)pos.y - 25, 8, skinColor);
    DrawCircle((int)pos.x - 15, (int)pos.y - 15, 8, skinColor);
    DrawCircle((int)pos.x + 15, (int)pos.y - 15, 8, skinColor);

    DrawText(isLeft ? "L (E)" : "P (O)", (int)pos.x - 12, (int)pos.y - 5, 12, BLACK);

    if (isHolding) {
        DrawCircle((int)pos.x, (int)pos.y, 10, RED);
    }
}

bool HandClick(const Hand& h, Rectangle rect)
{
    bool keyPressed = h.isLeft ? IsKeyPressed(KEY_E) : IsKeyPressed(KEY_O);
    return CheckCollisionPointRec(h.pos, rect) && keyPressed;
}