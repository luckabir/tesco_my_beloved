#ifndef PROFILE_H
#define PROFILE_H

#include <string>
#include <vector>

enum class PlayerRank
{
    NOVACEK = 0,
    ZAUCENY_BRIGADNIK = 1,
    POKROCILY_POKLADNI = 2,
    EXPERT_NA_PECIVO = 3
};

struct Profile
{
    std::string profilePicturePath = "ASSets/profile/default_avatar.png";
    std::string nickname = "";
    PlayerRank rank = PlayerRank::NOVACEK;

    int employeeId = 0;
    int pinCode = 0;
    int daysUntilPasswordChange = 30;

    int maxScore = 0;              // nejvic vydelano za jednu smenu
    int save_id = 0;

    int shiftsCompleted = 0;       // uspesne dokoncene smeny
    int customersServed = 0;       // celkem odbaveno lidi
    int totalMoneyEarned = 0;      // celkem vydelano bez vyhazovu

    int currentDayStreak = 0;      // aktualni serie smen bez vyhazovu
    int bestDayStreak = 0;         // nejdelsi serie smen bez vyhazovu

    std::string GetRankName() const;
    void UpdateRank();
};

extern Profile activeProfile;
extern bool isUserLoggedIn;
extern std::vector<std::string> allProfiles;

void CreateProfile(const std::string& profileName, int pin);
bool LoadProfile(const std::string& profileName, int enteredPin);
void LoadProfilesList();
void DeleteProfile(const std::string& profileName);
void SaveProfile();
void LogoutProfile();

#endif