#include "scene_score.h"
#include "../managers/InputManager.h"
#include "../managers/AssetManager.h"
#include "../structures/Profile.h"
#include "raylib.h"

#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

static bool LoadScoreProfile(const std::string& profileName, ScoreRow& row)
{
    std::string filename = "profiles/profil_" + profileName + ".txt";
    std::ifstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    std::string profilePicturePath;
    std::string nickname;
    int rankAsInt;
    int employeeId;
    int pinCode;
    int daysUntilPasswordChange;
    int maxScore;
    int save_id;
    int shiftsCompleted;
    int customersServed;
    int totalMoneyEarned;
    int currentDayStreak;
    int bestDayStreak;

    file >> profilePicturePath;
    file >> nickname;
    file >> rankAsInt;
    file >> employeeId;
    file >> pinCode;
    file >> daysUntilPasswordChange;
    file >> maxScore;
    file >> save_id;
    file >> shiftsCompleted;
    file >> customersServed;

    if (!(file >> totalMoneyEarned)) {
        totalMoneyEarned = 0;
        file.clear();
    }

    if (!(file >> currentDayStreak)) {
        currentDayStreak = 0;
        file.clear();
    }

    if (!(file >> bestDayStreak)) {
        bestDayStreak = 0;
        file.clear();
    }

    row.nickname = nickname;
    row.bestShift = maxScore;
    row.bestStreak = bestDayStreak;

    return true;
}

static std::vector<ScoreRow> LoadScoreRows()
{
    std::vector<ScoreRow> rows;

    LoadProfilesList();

    for (const std::string& profileName : allProfiles) {
        ScoreRow row;

        if (LoadScoreProfile(profileName, row)) {
            rows.push_back(row);
        }
    }

    if (rows.empty() && isUserLoggedIn) {
        ScoreRow row;
        row.nickname = activeProfile.nickname;
        row.bestShift = activeProfile.maxScore;
        row.bestStreak = activeProfile.bestDayStreak;

        rows.push_back(row);
    }

    return rows;
}

static void DrawScoreTable(
    const char* title,
    std::vector<ScoreRow> rows,
    int x,
    int y,
    int mode
)
{
    if (mode == 0) {
        std::sort(rows.begin(), rows.end(), [](const ScoreRow& a, const ScoreRow& b) {
            return a.bestShift > b.bestShift;
        });
    } else {
        std::sort(rows.begin(), rows.end(), [](const ScoreRow& a, const ScoreRow& b) {
            return a.bestStreak > b.bestStreak;
        });
    }

    DrawTextEx(
        AssetManager::mainFont,
        title,
        Vector2{ (float)x, (float)y },
        15.0f,
        1.0f,
        BLACK
    );

    DrawRectangle(x, y + 30, 230, 260, LIGHTGRAY);
    DrawRectangleLines(x, y + 30, 230, 260, DARKGRAY);

    if (rows.empty()) {
        DrawTextEx(
            AssetManager::mainFont,
            "Zadne vysledky",
            Vector2{ (float)x + 20, (float)y + 80 },
            12.0f,
            1.0f,
            DARKGRAY
        );

        return;
    }

    int maxRows = rows.size() < 5 ? (int)rows.size() : 5;

    for (int i = 0; i < maxRows; i++) {
        const ScoreRow& row = rows[i];

        int value = 0;
        std::string suffix;

        if (mode == 0) {
            value = row.bestShift;
            suffix = " Kc";
        } else {
            value = row.bestStreak;
            suffix = " dni";
        }

        Color valueColor = (i == 0) ? RED : BLUE;

        std::string namePart = std::to_string(i + 1) + ". " + row.nickname;
        std::string valuePart = std::to_string(value) + suffix;

        float rowY = (float)y + 65 + i * 38.0f;

        DrawTextEx(
            AssetManager::mainFont,
            namePart.c_str(),
            Vector2{ (float)x + 12, rowY },
            12.0f,
            1.0f,
            BLACK
        );

        DrawTextEx(
            AssetManager::mainFont,
            valuePart.c_str(),
            Vector2{ (float)x + 135, rowY },
            12.0f,
            1.0f,
            valueColor
        );
    }
}

void runScore(GameState &currentState, InputManager &input)
{
    if (input.IsBackTriggered()) {
        currentState = STATE_MENU;
        return;
    }

    Vector2 mousePos = GetMousePosition();

    float scale = fminf(
        (float)GetScreenWidth() / 800.0f,
        (float)GetScreenHeight() / 600.0f
    );

    mousePos.x = (mousePos.x - ((float)GetScreenWidth() - (800.0f * scale)) * 0.5f) / scale;
    mousePos.y = (mousePos.y - ((float)GetScreenHeight() - (600.0f * scale)) * 0.5f) / scale;

    ClearBackground(RAYWHITE);

    std::vector<ScoreRow> rows = LoadScoreRows();

    DrawTextEx(
        AssetManager::mainFont,
        "STATISTIKY PROFILU",
        Vector2{ 205, 45 },
        22.0f,
        1.0f,
        BLACK
    );

    DrawScoreTable(
        "NEJVIC ZA DEN",
        rows,
        140,
        115,
        0
    );

    DrawScoreTable(
        "NEJVIC DNI",
        rows,
        430,
        115,
        1
    );

    if (isUserLoggedIn) {
        DrawRectangle(115, 430, 570, 70, Fade(SKYBLUE, 0.35f));
        DrawRectangleLines(115, 430, 570, 70, DARKBLUE);

        DrawTextEx(
            AssetManager::mainFont,
            TextFormat("Tvuj profil: %s", activeProfile.nickname.c_str()),
            Vector2{ 140, 445 },
            12.0f,
            1.0f,
            BLACK
        );

        DrawTextEx(
            AssetManager::mainFont,
            TextFormat(
                "Nejvic za den: %d Kc   Nejvic dni: %d",
                activeProfile.maxScore,
                activeProfile.bestDayStreak
            ),
            Vector2{ 140, 472 },
            11.0f,
            1.0f,
            DARKGRAY
        );
    }

    Rectangle backBtn = { 250, 530, 300, 40 };
    bool hoverBack = CheckCollisionPointRec(mousePos, backBtn);

    DrawRectangleRec(backBtn, hoverBack ? BLUE : DARKBLUE);

    DrawTextEx(
        AssetManager::mainFont,
        "ZPET DO MENU",
        Vector2{ backBtn.x + 85, backBtn.y + 12 },
        14.0f,
        1.0f,
        WHITE
    );

    if (hoverBack && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        currentState = STATE_MENU;
    }
}