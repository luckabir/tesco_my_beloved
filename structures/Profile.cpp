#include "Profile.h"
#include "Settings.h"
#include <fstream>
#include <cstdlib> 
#include <algorithm>
#include <filesystem>

Profile activeProfile;
bool isUserLoggedIn = false;
std::vector<std::string> allProfiles;

std::string Profile::GetRankName() const
{
    switch (rank) {
        case PlayerRank::NOVACEK:
            return "Uplny novacek";
        case PlayerRank::ZAUCENY_BRIGADNIK:
            return "Zauceny brigadnik";
        case PlayerRank::JUNIOR_POKLADNI:
            return "Junior pokladni";
        case PlayerRank::POKROCILY_POKLADNI:
            return "Pokrocily pokladni";
        case PlayerRank::RYCHLA_RUKA:
            return "Rychla ruka";
        case PlayerRank::EXPERT_NA_PECIVO:
            return "Expert na pecivo";
        case PlayerRank::HLIDAC_CLUBCARD:
            return "Hlidac Clubcard";
        case PlayerRank::LOVEC_SLEV:
            return "Lovec slev";
        case PlayerRank::MISTR_MARKOVANI:
            return "Mistr markovani";
        case PlayerRank::OCHRANCE_PASU:
            return "Ochrance pasu";
        case PlayerRank::SPECIALISTA_NA_FRONTY:
            return "Specialista na fronty";
        case PlayerRank::VETERAN_RANNI_SICHTY:
            return "Veteran ranni sichty";
        case PlayerRank::POSTRACH_SAMOOBSLUZNYCH_POKLADEN:
            return "Postrach samoobsluznych pokladen";
        case PlayerRank::LEGENDA_OD_POKLADNY:
            return "Legenda od pokladny";
        case PlayerRank::KRAL_AKCNIHO_LETAKU:
            return "Kral akcniho letaku";
        case PlayerRank::VRCHNI_KROTITEL_ZAKAZNIKU:
            return "Vrchni krotitel zakazniku";
        case PlayerRank::SUPERVIZOR_SMENY:
            return "Supervizor smeny";
        case PlayerRank::MANAZER_PROVOZU:
            return "Manazer provozu";
        case PlayerRank::REGIONALNI_LEGENDA:
            return "Regionalni legenda";
        case PlayerRank::BOZSTVO_TESCO_KASY:
            return "Bozstvo Tesco kasy";
        default:
            return "Otrok korporatu";
    }
}

void Profile::UpdateRank()
{
    if (maxScore >= 3000) rank = PlayerRank::BOZSTVO_TESCO_KASY;
    else if (maxScore >= 2800) rank = PlayerRank::REGIONALNI_LEGENDA;
    else if (maxScore >= 2600) rank = PlayerRank::MANAZER_PROVOZU;
    else if (maxScore >= 2400) rank = PlayerRank::SUPERVIZOR_SMENY;
    else if (maxScore >= 2200) rank = PlayerRank::VRCHNI_KROTITEL_ZAKAZNIKU;
    else if (maxScore >= 2000) rank = PlayerRank::KRAL_AKCNIHO_LETAKU;
    else if (maxScore >= 1800) rank = PlayerRank::LEGENDA_OD_POKLADNY;
    else if (maxScore >= 1600) rank = PlayerRank::POSTRACH_SAMOOBSLUZNYCH_POKLADEN;
    else if (maxScore >= 1450) rank = PlayerRank::VETERAN_RANNI_SICHTY;
    else if (maxScore >= 1300) rank = PlayerRank::SPECIALISTA_NA_FRONTY;
    else if (maxScore >= 1150) rank = PlayerRank::OCHRANCE_PASU;
    else if (maxScore >= 1000) rank = PlayerRank::MISTR_MARKOVANI;
    else if (maxScore >= 850) rank = PlayerRank::LOVEC_SLEV;
    else if (maxScore >= 700) rank = PlayerRank::HLIDAC_CLUBCARD;
    else if (maxScore >= 550) rank = PlayerRank::EXPERT_NA_PECIVO;
    else if (maxScore >= 400) rank = PlayerRank::RYCHLA_RUKA;
    else if (maxScore >= 300) rank = PlayerRank::POKROCILY_POKLADNI;
    else if (maxScore >= 200) rank = PlayerRank::JUNIOR_POKLADNI;
    else if (maxScore >= 100) rank = PlayerRank::ZAUCENY_BRIGADNIK;
    else rank = PlayerRank::NOVACEK;
}

void CreateProfile(const std::string& profileName, int pin)
{
    std::filesystem::create_directories("profiles");
    activeProfile.nickname = profileName;
    activeProfile.pinCode = pin;
    activeProfile.employeeId = 1000 + (rand() % 9000); 
    activeProfile.daysUntilPasswordChange = 30; 
    activeProfile.maxScore = 0;
    activeProfile.shiftsCompleted = 0;
    activeProfile.customersServed = 0;
    activeProfile.profilePicturePath = "ASSets/profile/default_avatar.png"; 
    activeProfile.rank = PlayerRank::NOVACEK;
    activeProfile.save_id = 0;
    activeProfile.totalMoneyEarned = 0;
    activeProfile.currentDayStreak = 0;
    activeProfile.bestDayStreak = 0;

    isUserLoggedIn = true;
    SaveProfile();
    SaveSettings();

    if (std::find(allProfiles.begin(), allProfiles.end(), profileName) == allProfiles.end())
    {
        allProfiles.push_back(profileName);
        std::ofstream listFile("profiles/profiles_list.txt");
        for (const auto& name : allProfiles) {
            listFile << name << "\n";
        }
    }
}


bool LoadProfile(const std::string& profileName, int enteredPin){
    std::string filename = "profiles/profil_" + profileName + ".txt";
    std::ifstream file(filename);

    if (file.is_open()){
        Profile temp;
        int rankAsInt;

        file >> temp.profilePicturePath;
        file >> temp.nickname;
        file >> rankAsInt;
        temp.rank = (PlayerRank)rankAsInt;
        file >> temp.employeeId;
        file >> temp.pinCode;
        file >> temp.daysUntilPasswordChange;
        file >> temp.maxScore;
        file >> temp.save_id;
        file >> temp.shiftsCompleted;
        file >> temp.customersServed;
        file >> temp.totalMoneyEarned;
        if (!(file >> temp.currentDayStreak)) {
            temp.currentDayStreak = 0;
            file.clear();
        }

        if (!(file >> temp.bestDayStreak)) {
            temp.bestDayStreak = 0;
            file.clear();
        }

        file.close();


        if (temp.pinCode == enteredPin){
            activeProfile = temp;
            isUserLoggedIn = true;
            return true; 
        }
    }
    return false; 
}

void LoadProfilesList(){
    allProfiles.clear();
    std::ifstream file("profiles/profiles_list.txt");
    std::string name;
    if (file.is_open()){
        while (file >> name){
            allProfiles.push_back(name);
        }
        file.close();
    }
}

void DeleteProfile(const std::string& profileName){
    allProfiles.erase(
        std::remove_if(allProfiles.begin(), allProfiles.end(), [&profileName](const std::string& name) { return name == profileName; }),  allProfiles.end()
    );
    
    std::ofstream listFile("profiles/profiles_list.txt");
    if (listFile.is_open()){
        for (const auto& name : allProfiles) {
            listFile << name << "\n";
        }
        listFile.close();
    }

    std::string filename = "profiles/profil_" + profileName + ".txt";
    ::remove(filename.c_str()); 
    if (activeProfile.nickname == profileName) {
        LogoutProfile();
    }
}

void SaveProfile(){
    if (!isUserLoggedIn) return;

    std::string filename = "profiles/profil_" + activeProfile.nickname + ".txt";
    std::ofstream file(filename);

    if (file.is_open()){
        file << activeProfile.profilePicturePath << "\n";
        file << activeProfile.nickname << "\n";
        file << (int)activeProfile.rank << "\n";
        file << activeProfile.employeeId << "\n";
        file << activeProfile.pinCode << "\n";
        file << activeProfile.daysUntilPasswordChange << "\n";
        file << activeProfile.maxScore << "\n";
        file << activeProfile.save_id << "\n";
        file << activeProfile.shiftsCompleted << "\n";
        file << activeProfile.customersServed << "\n";
        file << activeProfile.totalMoneyEarned << "\n";
        file << activeProfile.currentDayStreak << "\n";
        file << activeProfile.bestDayStreak << "\n";
        file.close();
    }
}

void LogoutProfile(){
    SaveProfile();    
    activeProfile = Profile(); 
    isUserLoggedIn = false;
    SaveSettings();
}