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
    std::string profilePicturePath;
    std::string nickname;
    PlayerRank rank;
    int employeeId;
    int pinCode;
    int daysUntilPasswordChange;
    int maxScore;
    int save_id;
    int shiftsCompleted;
    int customersServed;

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