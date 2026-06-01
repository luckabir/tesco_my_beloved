#include "Item.h"

Item::Item(
    std::string n,
    int p,
    int cp,
    Texture2D tex,
    Color fallback,
    Vector2 startPos,
    ItemCategory cat
)
    : name(n),
      basePrice(p),
      clubcardPrice(cp),
      texture(tex),
      fallbackColor(fallback),
      pos(startPos),
      isScanned(false),
      missedPenaltyGiven(false),
      category(cat),
      drawWidth(90.0f),
      drawHeight(60.0f)
{
    CalculateDrawSize();
}

void Item::CalculateDrawSize()
{
    const float maxW = 150.0f;
    const float maxH = 120.0f;

    if (texture.id <= 0 || texture.width <= 0 || texture.height <= 0) {
        drawWidth = 90.0f;
        drawHeight = 60.0f;
        return;
    }

    float scaleX = maxW / (float)texture.width;
    float scaleY = maxH / (float)texture.height;
    float scale = scaleX < scaleY ? scaleX : scaleY;

    drawWidth = texture.width * scale;
    drawHeight = texture.height * scale;

    if (drawWidth < 40.0f && drawHeight < 40.0f) {
        drawWidth *= 1.3f;
        drawHeight *= 1.3f;
    }

    if (category == BAKERY) {
        drawWidth *= 0.85f;
        drawHeight *= 0.85f;
    }

    if (category == RESTRICTED_18) {
        drawWidth *= 0.9f;
        drawHeight *= 1.05f;
    }

    if (category == WEIGHED) {
        drawWidth *= 0.9f;
        drawHeight *= 0.9f;
    }
}

Rectangle Item::getRect() const
{
    return Rectangle{
        pos.x,
        pos.y,
        drawWidth,
        drawHeight
    };
}

void Item::Draw()
{
    Rectangle dest = getRect();

    if (texture.id > 0) {
        Rectangle source = {
            0.0f,
            0.0f,
            (float)texture.width,
            (float)texture.height
        };

        DrawTexturePro(
            texture,
            source,
            dest,
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(dest, fallbackColor);
        DrawRectangleLines(
            (int)dest.x,
            (int)dest.y,
            (int)dest.width,
            (int)dest.height,
            BLACK
        );

        DrawText(
            name.c_str(),
            (int)pos.x + 5,
            (int)pos.y + 12,
            10,
            fallbackColor.r > 200 && fallbackColor.g > 200 ? BLACK : WHITE
        );
    }

    if (isScanned) {
        DrawText("[OK]", (int)pos.x + 12, (int)pos.y - 12, 10, GREEN);
    }
}

bool Item::requiresSpecialAction()
{
    return false;
}