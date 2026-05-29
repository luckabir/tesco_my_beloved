#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "raylib.h"
#include <string>

enum CustomerState {
    WALKING_IN,
    WAITING,
    PAYING,
    WALKING_OUT,
    GONE
};

class Customer {
public:
    std::string name;
    int age;
    bool hasClubcard;
    bool hasCheckedCard;
    bool gaveClubcard;
    std::string cardResponse;

    Vector2 pos;
    CustomerState state;

    Customer(std::string n, int a, bool hasCard);

    void Update();
    void Draw() const;
};

#endif