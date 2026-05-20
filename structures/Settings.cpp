#include "Settings.h"
#include "Profile.h"
#include <fstream>


Settings gameSettings;

void SaveSettings()
{
    std::ofstream file("settings.txt");

    if (file.is_open())
    {
        file << gameSettings.volume << std::endl;
        file << gameSettings.fullscreen << std::endl;
        if (isUserLoggedIn) {
            file << activeProfile.nickname << std::endl;
        } else {
            file << "NONE" << std::endl;
        }
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
        file >> gameSettings.lastLoggedInUser;
        file.close();
        if (gameSettings.lastLoggedInUser != "NONE" && !gameSettings.lastLoggedInUser.empty()) 
        {
            std::string filename = "profiles/profil_" + gameSettings.lastLoggedInUser + ".txt";
            std::ifstream profFile(filename);
            if (profFile.is_open()) 
            {
                int rankAsInt;
                profFile >> activeProfile.profilePicturePath;
                profFile >> activeProfile.nickname;
                profFile >> rankAsInt;
                activeProfile.rank = (PlayerRank)rankAsInt;
                profFile >> activeProfile.employeeId;
                profFile >> activeProfile.pinCode;
                profFile >> activeProfile.daysUntilPasswordChange;
                profFile >> activeProfile.maxScore;
                profFile >> activeProfile.save_id;
                profFile >> activeProfile.shiftsCompleted;
                profFile >> activeProfile.customersServed;
                profFile.close();
                isUserLoggedIn = true; 
            }
        }
    }
    else
    {
        gameSettings.volume = 0.5f;
        gameSettings.fullscreen = false;
        gameSettings.lastLoggedInUser = "NONE";
        SaveSettings();
    }
}