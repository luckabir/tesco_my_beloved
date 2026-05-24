#include "AssetManager.h"
#include <fstream>
#include <sstream>

Font AssetManager::mainFont = { 0 };
Music AssetManager::menuMusic = { 0 };
Music AssetManager::gameMusic = { 0 };
Music* AssetManager::currentMusic = nullptr;
std::vector<ItemTemplate> AssetManager::itemDatabase;
std::map<std::string, Texture2D> AssetManager::textures;

void AssetManager::LoadAll(){
    mainFont = LoadFontEx("ASSets/fonts/Daydream.otf", 64, nullptr, 0);
    menuMusic = LoadMusicStream("ASSets/sounds/menu.mp3");
    gameMusic = LoadMusicStream("ASSets/sounds/game.mp3");
    menuMusic.looping = true;
    gameMusic.looping = true;
}

void AssetManager::UnloadAll(){
    StopAllMusic();
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(gameMusic);
    UnloadFont(mainFont);
    for (auto const& [id, tex] : textures) {
        UnloadTexture(tex);
    }
    textures.clear();
}

void AssetManager::UpdateAudio(){
    if (currentMusic != nullptr){
        UpdateMusicStream(*currentMusic);
    }
}

void AssetManager::SetActiveMusic(MusicType type)
{
    if (type == MUSIC_MENU && currentMusic == &menuMusic)
        return;

    if (type == MUSIC_GAME && currentMusic == &gameMusic)
        return;

    if (type == MUSIC_NONE && currentMusic == nullptr)
        return;

    StopMusicStream(menuMusic);
    StopMusicStream(gameMusic);

    currentMusic = nullptr;

    switch (type){
        case MUSIC_MENU:
            PlayMusicStream(menuMusic);
            currentMusic = &menuMusic;
            break;

        case MUSIC_GAME:
            PlayMusicStream(gameMusic);
            currentMusic = &gameMusic;
            break;

        default:
            break;
    }
}

void AssetManager::StopAllMusic(){
    StopMusicStream(menuMusic);
    StopMusicStream(gameMusic);
    currentMusic = nullptr;
}

void AssetManager::PlayMenuMusic(){
    if (currentMusic == &menuMusic)
        return;

    StopAllMusic();
    PlayMusicStream(menuMusic);
    currentMusic = &menuMusic;
}

void AssetManager::PlayGameMusic(){
    if (currentMusic == &gameMusic)
        return;

    StopAllMusic();
    PlayMusicStream(gameMusic);
    currentMusic = &gameMusic;
}

void AssetManager::LoadItemsCSV(const std::string& path) {
    std::ifstream file(path);
    std::string line;

    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id, name, categoryStr, priceStr, clubPriceStr;

        std::getline(ss, id, ';');
        std::getline(ss, name, ';');
        std::getline(ss, categoryStr, ';');
        std::getline(ss, priceStr, ';');
        std::getline(ss, clubPriceStr, ';');

        ItemCategory cat = NORMAL;
        if (categoryStr == "RESTRICTED_18") cat = RESTRICTED_18;
        else if (categoryStr == "BAKERY") cat = BAKERY;
        else if (categoryStr == "WEIGHED") cat = WEIGHED;

        ItemTemplate tmpl;
        tmpl.id = id;
        tmpl.name = name;
        tmpl.category = cat;
        tmpl.basePrice = std::stoi(priceStr);
        tmpl.clubcardPrice = std::stoi(clubPriceStr);

        itemDatabase.push_back(tmpl);
    }
}

ItemTemplate AssetManager::GetRandomItemTemplate() {
    int randomIndex = GetRandomValue(0, itemDatabase.size() - 1);
    return itemDatabase[randomIndex];
}

Texture2D AssetManager::GetTexture(const std::string& id) {
    if (textures.find(id) != textures.end()) {
        return textures[id];
    }

    std::string path = "ASSets/textures/items/" + id + ".png";
    Texture2D tex = LoadTexture(path.c_str());

    textures[id] = tex;

    return tex;
}