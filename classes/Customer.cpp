#include "Customer.h"

Customer::Customer(
    std::string id,
    std::string name,
    int age,
    bool hasClubcard,
    bool isRegular,
    CustomerArchetype archetype,
    int maxPatience
)
    : id(id),
      name(name),
      age(age),
      hasClubcard(hasClubcard),
      hasCheckedCard(false),
      gaveClubcard(false),
      cardResponse(""),
      isRegular(isRegular),
      archetype(archetype),
      maxPatience(maxPatience),
      patience((float)maxPatience),
      mood(MOOD_NEUTRAL),
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

void Customer::UpdatePatience(float dt)
{
    if (state != WAITING && state != PAYING) {
        return;
    }

    patience -= dt;

    if (patience < 0.0f) {
        patience = 0.0f;
    }

    float ratio = patience / (float)maxPatience;

    if (ratio > 0.70f) {
        mood = MOOD_HAPPY;
    }
    else if (ratio > 0.40f) {
        mood = MOOD_NEUTRAL;
    }
    else if (ratio > 0.15f) {
        mood = MOOD_ANNOYED;
    }
    else {
        mood = MOOD_ANGRY;
    }
}

void Customer::ChangePatience(int amount)
{
    patience += amount;

    if (patience > maxPatience) {
        patience = (float)maxPatience;
    }

    if (patience < 0.0f) {
        patience = 0.0f;
    }
}

bool Customer::IsOutOfPatience() const
{
    return patience <= 0.0f;
}

const char* Customer::GetMoodText() const
{
    switch (mood) {
        case MOOD_HAPPY: return ":)";
        case MOOD_NEUTRAL: return ":|";
        case MOOD_ANNOYED: return ">:(";
        case MOOD_ANGRY: return "!!!";
        default: return ":|";
    }
}

void Customer::DrawPatienceBar() const
{
    float ratio = patience / (float)maxPatience;

    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    int barX = (int)pos.x;
    int barY = (int)pos.y - 90;
    int barW = 100;
    int barH = 10;

    DrawRectangle(barX, barY, barW, barH, DARKGRAY);

    Color barColor = GREEN;
    if (ratio < 0.40f) barColor = ORANGE;
    if (ratio < 0.20f) barColor = RED;

    DrawRectangle(barX, barY, (int)(barW * ratio), barH, barColor);
    DrawRectangleLines(barX, barY, barW, barH, BLACK);

    DrawText(GetMoodText(), barX + 110, barY - 6, 20, BLACK);
}

void Customer::Draw() const
{
    DrawRectangle((int)pos.x, (int)pos.y, 100, 350, DARKBLUE);
    DrawCircle((int)pos.x + 50, (int)pos.y - 30, 40, BEIGE);

    DrawText(name.c_str(), (int)pos.x, (int)pos.y - 120, 14, BLACK);
    DrawText(TextFormat("Vek %d", age), (int)pos.x + 10, (int)pos.y - 30, 16, MAROON);

    DrawPatienceBar();

    if (gaveClubcard) {
        DrawRectangle((int)pos.x + 70, (int)pos.y + 60, 30, 20, ORANGE);
        DrawText("KARTA", (int)pos.x + 72, (int)pos.y + 65, 8, BLACK);
    }
    DrawSpeechBubble();
}

static std::string PickRandomLine(const std::vector<std::string>& lines)
{
    if (lines.empty()) {
        return "";
    }

    return lines[GetRandomValue(0, (int)lines.size() - 1)];
}

void Customer::SayArrivalLine()
{
    speechText = PickRandomLine(arrivalLines);
    speechTimer = 3.0f;
}

void Customer::SayExitLine()
{
    std::string line;

    if (mood == MOOD_HAPPY) {
        line = PickRandomLine(exitHappyLines);
    }
    else if (mood == MOOD_NEUTRAL) {
        line = PickRandomLine(exitNeutralLines);
    }
    else if (mood == MOOD_ANNOYED) {
        line = PickRandomLine(exitAnnoyedLines);
    }
    else if (mood == MOOD_ANGRY) {
        line = PickRandomLine(exitAngryLines);
    }

    if (line.empty()) {
        line = "Zakaznik odchazi...";
    }

    speechText = line;
    speechTimer = 3.0f;
}

void Customer::UpdateSpeech(float dt)
{
    if (speechTimer > 0.0f) {
        speechTimer -= dt;

        if (speechTimer < 0.0f) {
            speechTimer = 0.0f;
        }
    }
}

void Customer::DrawSpeechBubble() const
{
    if (speechTimer <= 0.0f || speechText.empty()) {
        return;
    }

    int x = (int)pos.x - 20;
    int y = (int)pos.y - 150;

    DrawRectangle(x, y, 220, 45, Fade(RAYWHITE, 0.95f));
    DrawRectangleLines(x, y, 220, 45, DARKGRAY);
    DrawText(speechText.c_str(), x + 10, y + 15, 12, BLACK);
}