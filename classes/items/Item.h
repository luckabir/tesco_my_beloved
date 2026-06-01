#ifndef ITEM_H
#define ITEM_H

#include "../../managers/AssetManager.h"
#include "raylib.h"
#include <string>

class Item {
public:
    std::string name;
    int basePrice;
    int clubcardPrice;
    Texture2D texture;
    Color fallbackColor;
    Vector2 pos;
    bool isScanned;
    bool missedPenaltyGiven;
    ItemCategory category;

    float drawWidth;
    float drawHeight;

    Item(
        std::string n,
        int p,
        int cp,
        Texture2D tex,
        Color fallback,
        Vector2 startPos,
        ItemCategory cat = NORMAL
    );

    virtual ~Item() = default;

    void CalculateDrawSize();
    Rectangle getRect() const;
    void Draw();

    virtual bool requiresSpecialAction();
};

#endif