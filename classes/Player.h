#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

struct Hand {
    Vector2 pos;
    bool isHolding;
    int holdingItemIndex;
    Color skinColor;
    bool isLeft;

    void Draw() const;
};

bool HandClick(const Hand& h, Rectangle rect);

#endif