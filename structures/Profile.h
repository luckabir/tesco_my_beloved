#ifndef PROFILE_H
#define PROFILE_H

#include <string>
#include <vector>

enum class PlayerRank{
    NOVACEK = 0,
    ZAUCENY_BRIGADNIK = 1,
    JUNIOR_POKLADNI = 2,
    POKROCILY_POKLADNI = 3,
    RYCHLA_RUKA = 4,
    EXPERT_NA_PECIVO = 5,
    HLIDAC_CLUBCARD = 6,
    LOVEC_SLEV = 7,
    MISTR_MARKOVANI = 8,
    OCHRANCE_PASU = 9,
    SPECIALISTA_NA_FRONTY = 10,
    VETERAN_RANNI_SICHTY = 11,
    POSTRACH_SAMOOBSLUZNYCH_POKLADEN = 12,
    LEGENDA_OD_POKLADNY = 13,
    KRAL_AKCNIHO_LETAKU = 14,
    VRCHNI_KROTITEL_ZAKAZNIKU = 15,
    SUPERVIZOR_SMENY = 16,
    MANAZER_PROVOZU = 17,
    REGIONALNI_LEGENDA = 18,
    BOZSTVO_TESCO_KASY = 19
};

struct Profile{
    std::string profilePicturePath = "ASSets/profile/default_avatar.png";
    std::string nickname = "";
    PlayerRank rank = PlayerRank::NOVACEK;
    int employeeId = 0;
    int pinCode = 0;
    int daysUntilPasswordChange = 30;
    int maxScore = 0;           
    int save_id = 0;
    int shiftsCompleted = 0;      
    int customersServed = 0;     
    int totalMoneyEarned = 0;     
    int currentDayStreak = 0;      
    int bestDayStreak = 0;       
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