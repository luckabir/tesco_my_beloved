#include "Customer.h"

Customer::Customer(std::string n, int a, bool hasCard)
    : name(n),
      age(a),
      hasClubcard(hasCard),
      hasCheckedCard(false),
      gaveClubcard(false),
      cardResponse(""),
      pos(Vector2{-100.0f, 200.0f}),
      state(WALKING_IN)
{
    if (hasClubcard && GetRandomValue(1, 100) > 70) {
        gaveClubcard = true;
    }
}

void Customer::Update()
{
    if (state == WALKING_IN) {
        pos.x += 3.0f;

        if (pos.x >= 350) {
            state = WAITING;
        }
    }
    else if (state == WALKING_OUT) {
        pos.x += 4.0f;

        if (pos.x > 900) {
            state = GONE;
        }
    }
}

void Customer::Draw() const
{
    DrawRectangle((int)pos.x, (int)pos.y, 100, 350, DARKBLUE);
    DrawCircle((int)pos.x + 50, (int)pos.y - 30, 40, BEIGE);
    DrawText(TextFormat("Vek: %d", age), (int)pos.x + 10, (int)pos.y + 20, 16, WHITE);

    if (gaveClubcard) {
        DrawRectangle((int)pos.x + 70, (int)pos.y + 60, 30, 20, ORANGE);
        DrawText("KARTA", (int)pos.x + 72, (int)pos.y + 65, 8, BLACK);
    }
}