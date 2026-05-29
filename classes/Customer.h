#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "raylib.h"
#include "QTEEvent.h"
#include <string>
#include <vector>

enum CustomerState {
    WALKING_IN,
    WAITING,
    PAYING,
    WALKING_OUT,
    GONE
};

class Customer {
public:
    std::string id;
    std::string name;

    int age;
    bool hasClubcard;
    bool hasCheckedCard;
    bool gaveClubcard;
    std::string cardResponse;

    bool isRegular;
    CustomerArchetype archetype;

    int maxPatience;
    float patience;
    CustomerMood mood;

    std::vector<std::string> favoriteItemIds;
    std::vector<QTEEvent> qteSet;

    Vector2 pos;
    CustomerState state;

    std::vector<std::string> arrivalLines;

    std::vector<std::string> exitHappyLines;
    std::vector<std::string> exitNeutralLines;
    std::vector<std::string> exitAnnoyedLines;
    std::vector<std::string> exitAngryLines;

    std::string speechText;
    float speechTimer;

    Customer(
        std::string id,
        std::string name,
        int age,
        bool hasClubcard,
        bool isRegular,
        CustomerArchetype archetype,
        int maxPatience
    );



    void Update();
    void UpdatePatience(float dt);
    void ChangePatience(int amount);
    bool IsOutOfPatience() const;
    void SayArrivalLine();
    void Draw() const;
    void DrawPatienceBar() const;
    void SayExitLine();
    void UpdateSpeech(float dt);
    void DrawSpeechBubble() const;

    const char* GetMoodText() const;
};

#endif