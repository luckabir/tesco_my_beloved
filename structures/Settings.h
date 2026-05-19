#pragma once

struct Settings {
    float volume;
    bool fullscreen;
};

extern Settings gameSettings;

void SaveSettings();
void LoadSettings();
