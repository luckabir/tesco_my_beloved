#include "Profile.h"
#include "Settings.h"
#include <fstream>
#include <cstdlib> 
#include <algorithm>

Profile activeProfile;
bool isUserLoggedIn = false;
std::vector<std::string> allProfiles;

std::string Profile::GetRankName() const
{
    switch (rank)
    {
        case PlayerRank::NOVACEK:
            return "Uplny novacek";
        case PlayerRank::ZAUCENY_BRIGADNIK:
            return "Zauceny brigadnik";
        case PlayerRank::POKROCILY_POKLADNI:
            return "Pokrocily pokladni";
        case PlayerRank::EXPERT_NA_PECIVO:
            return "Expert na pecivo";
        default:
            return "Otrok korporatu";
    }
}

void Profile::UpdateRank()
{
    if (maxScore >= 10000){
        rank = PlayerRank::EXPERT_NA_PECIVO;
    }else if (maxScore >= 5000){
        rank = PlayerRank::POKROCILY_POKLADNI;
    }else if (maxScore >= 1000){
        rank = PlayerRank::ZAUCENY_BRIGADNIK;
    }else{
        rank = PlayerRank::NOVACEK;
    }
}

void CreateProfile(const std::string& profileName, int pin)
{
    activeProfile.nickname = profileName;
    activeProfile.pinCode = pin;
    activeProfile.employeeId = 1000 + (rand() % 9000); 
    activeProfile.daysUntilPasswordChange = 30; 
    activeProfile.maxScore = 0;
    activeProfile.shiftsCompleted = 0;
    activeProfile.customersServed = 0;
    activeProfile.profilePicturePath = "../ASSets/profile/default_avatar.png"; 
    activeProfile.rank = PlayerRank::NOVACEK;
    activeProfile.save_id = 0;

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

        file.close();

        if (temp.pinCode == enteredPin){
            activeProfile = temp;
            isUserLoggedIn = true;
            return true; 
        }
    }
    return false; 
}

void LoadProfilesList()
{
    allProfiles.clear();
    std::ifstream file("profiles/profiles_list.txt");
    std::string name;
    if (file.is_open())
    {
        while (file >> name)
        {
            allProfiles.push_back(name);
        }
        file.close();
    }
}

void DeleteProfile(const std::string& profileName)
{
    allProfiles.erase(
        std::remove_if(allProfiles.begin(), allProfiles.end(), 
            [&profileName](const std::string& name) { return name == profileName; }), 
        allProfiles.end()
    );
    
    std::ofstream listFile("profiles/profiles_list.txt");
    if (listFile.is_open())
    {
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
        file.close();
    }
}

void LogoutProfile(){
    SaveProfile();    
    activeProfile = Profile(); 
    isUserLoggedIn = false;
    SaveSettings();
}