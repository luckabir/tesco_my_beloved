#pragma once
#include <string>

struct Settings {
    float volume;
    bool fullscreen;
    std::string lastLoggedInUser;
};

extern Settings gameSettings;

void SaveSettings();
void LoadSettings();
