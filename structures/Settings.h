#pragma once
#include <string>

struct Settings {
    float volume;
    bool fullscreen;
    std::string lastLoggedInUser;
    bool depressionMode;
};

extern Settings gameSettings;

void SaveSettings();
void LoadSettings();
