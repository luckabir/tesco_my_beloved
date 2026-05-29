#ifndef QTE_EVENT_H
#define QTE_EVENT_H

#include <string>

enum CustomerMood {
    MOOD_ANY,
    MOOD_HAPPY,
    MOOD_NEUTRAL,
    MOOD_ANNOYED,
    MOOD_ANGRY
};

enum CustomerArchetype {
    ARCH_NORMAL,
    ARCH_RUSHING,
    ARCH_TALKATIVE,
    ARCH_ANGRY,
    ARCH_CONFUSED,
    ARCH_REGULAR
};

struct QTEEvent {
    std::string id;
    std::string customerLine;

    int correctKey;
    float timeLimit;

    std::string successReply;
    std::string failReply;

    int patienceSuccess;
    int patienceFail;

    CustomerMood requiredMood;

    std::string optionR;
    std::string optionT;
    std::string optionZ;
    std::string optionU;
};

#endif