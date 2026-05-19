#include "../structures/Settings.h"
#include <fstream>

Settings gameSettings;

void SaveSettings()
{
    std::ofstream file("settings.txt");

    if (file.is_open())
    {
        file << gameSettings.volume << std::endl;
        file << gameSettings.fullscreen << std::endl;
        file.close();
    }
}

void LoadSettings()
{
    std::ifstream file("settings.txt");

    if (file.is_open())
    {
        file >> gameSettings.volume;
        file >> gameSettings.fullscreen;
        file.close();
    }
    else
    {
        gameSettings.volume = 0.5f;
        gameSettings.fullscreen = false;
        SaveSettings();
    }
}