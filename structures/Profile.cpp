#include "Profile.h"
#include "Settings.h"
#include <fstream>
#include <cstdlib> 
#include <algorithm>
#include <filesystem>

Profile activeProfile;
bool isUserLoggedIn = false;
std::vector<std::string> allProfiles;
static const std::string PROFILE_DIR = "ASSets/data/profiles";
static const std::string PROFILE_LIST_PATH = "ASSets/data/profiles/profiles_list.txt";

static std::string GetProfileFilePath(const std::string& profileName){
    return PROFILE_DIR + "/profil_" + profileName + ".txt";
}

std::string Profile::GetRankName() const {
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

void Profile::UpdateRank() {
    if (maxScore >= 200000) rank = PlayerRank::BOZSTVO_TESCO_KASY;
    else if (maxScore >= 150000) rank = PlayerRank::REGIONALNI_LEGENDA;
    else if (maxScore >= 100000) rank = PlayerRank::MANAZER_PROVOZU;
    else if (maxScore >= 90000) rank = PlayerRank::SUPERVIZOR_SMENY;
    else if (maxScore >= 80000) rank = PlayerRank::VRCHNI_KROTITEL_ZAKAZNIKU;
    else if (maxScore >= 70000) rank = PlayerRank::KRAL_AKCNIHO_LETAKU;
    else if (maxScore >= 65000) rank = PlayerRank::LEGENDA_OD_POKLADNY;
    else if (maxScore >= 60000) rank = PlayerRank::POSTRACH_SAMOOBSLUZNYCH_POKLADEN;
    else if (maxScore >= 55000) rank = PlayerRank::VETERAN_RANNI_SICHTY;
    else if (maxScore >= 50000) rank = PlayerRank::SPECIALISTA_NA_FRONTY;
    else if (maxScore >= 45000) rank = PlayerRank::OCHRANCE_PASU;
    else if (maxScore >= 40000) rank = PlayerRank::MISTR_MARKOVANI;
    else if (maxScore >= 35000) rank = PlayerRank::LOVEC_SLEV;
    else if (maxScore >= 30000) rank = PlayerRank::HLIDAC_CLUBCARD;
    else if (maxScore >= 25000) rank = PlayerRank::EXPERT_NA_PECIVO;
    else if (maxScore >= 20000) rank = PlayerRank::RYCHLA_RUKA;
    else if (maxScore >= 15000) rank = PlayerRank::POKROCILY_POKLADNI;
    else if (maxScore >= 10000) rank = PlayerRank::JUNIOR_POKLADNI;
    else if (maxScore >= 5000) rank = PlayerRank::ZAUCENY_BRIGADNIK;
    else rank = PlayerRank::NOVACEK;
}

void CreateProfile(const std::string& profileName, int pin) {
    std::filesystem::create_directories(PROFILE_DIR);
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
        std::ofstream listFile(PROFILE_LIST_PATH);
        for (const auto& name : allProfiles) {
            listFile << name << "\n";
        }
    }
}


bool LoadProfile(const std::string& profileName, int enteredPin){
    std::string filename = GetProfileFilePath(profileName);
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
    std::ifstream file(PROFILE_LIST_PATH);
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
    
    std::ofstream listFile(PROFILE_LIST_PATH);
    if (listFile.is_open()){
        for (const auto& name : allProfiles) {
            listFile << name << "\n";
        }
        listFile.close();
    }

    std::string filename = GetProfileFilePath(profileName);;
    ::remove(filename.c_str()); 
    if (activeProfile.nickname == profileName) {
        LogoutProfile();
    }
}

void SaveProfile(){
    if (!isUserLoggedIn) return;

    std::filesystem::create_directories(PROFILE_DIR);
    std::string filename = GetProfileFilePath(activeProfile.nickname);
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