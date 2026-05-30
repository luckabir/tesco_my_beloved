#include "AssetManager.h"
#include <fstream>
#include <sstream>

static bool itemTexturePreloadFinished = false;
Font AssetManager::mainFont = { 0 };
Sound AssetManager::menuMusic = { 0 };
Sound AssetManager::gameMusic = { 0 };
Sound* AssetManager::currentMusic = nullptr;
std::vector<ItemTemplate> AssetManager::itemDatabase;
std::map<std::string, Texture2D> AssetManager::textures;
std::map<std::string, Texture2D> AssetManager::customerTextures;

void AssetManager::LoadAll(){
    mainFont = LoadFontEx("ASSets/fonts/Daydream.otf", 64, nullptr, 0);
    menuMusic = LoadSound("ASSets/sounds/menu.wav");
    gameMusic = LoadSound("ASSets/sounds/game.wav");
}

void AssetManager::UnloadAll(){
    StopAllMusic();
    UnloadSound(menuMusic);
    UnloadSound(gameMusic);
    UnloadFont(mainFont);

    for (auto const& [id, tex] : textures) {
        UnloadTexture(tex);
    }
    textures.clear();

    for (auto const& [id, tex] : customerTextures) {
        UnloadTexture(tex);
    }
    customerTextures.clear();
}

void AssetManager::UpdateAudio()
{
    if (currentMusic != nullptr) {
        if (!IsSoundPlaying(*currentMusic)) {
            PlaySound(*currentMusic);
        }
    }
}

void AssetManager::SetActiveMusic(MusicType type)
{
    Sound* wantedMusic = nullptr;

    if (type == MUSIC_MENU) {
        wantedMusic = &menuMusic;
    }
    else if (type == MUSIC_GAME) {
        wantedMusic = &gameMusic;
    }
    else {
        wantedMusic = nullptr;
    }

    if (currentMusic == wantedMusic) {
        return;
    }

    if (currentMusic != nullptr) {
        StopSound(*currentMusic);
    }

    currentMusic = wantedMusic;

    if (currentMusic != nullptr) {
        PlaySound(*currentMusic);
    }
}

void AssetManager::StopAllMusic()
{
    StopSound(menuMusic);
    StopSound(gameMusic);
    currentMusic = nullptr;
}

void AssetManager::PlayMenuMusic()
{
    SetActiveMusic(MUSIC_MENU);
}

void AssetManager::PlayGameMusic()
{
    SetActiveMusic(MUSIC_GAME);
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

Texture2D AssetManager::GetTexture(const std::string& id)
{
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }

    if (itemTexturePreloadFinished) {
        TraceLog(LOG_WARNING, "MISSING PRELOADED ITEM TEXTURE, using fallback: %s", id.c_str());
        return Texture2D{ 0 };
    }

    std::string path = "ASSets/textures/items/" + id + ".png";

    TraceLog(LOG_INFO, "PRELOADING ITEM TEXTURE: %s", path.c_str());

    Texture2D tex = LoadTexture(path.c_str());
    textures[id] = tex;

    return tex;
}

void AssetManager::PreloadItemTextures()
{
    itemTexturePreloadFinished = false;

    for (const ItemTemplate& item : itemDatabase) {
        GetTexture(item.id);
    }

    itemTexturePreloadFinished = true;

    TraceLog(LOG_INFO, "ITEM TEXTURE PRELOAD FINISHED: %d textures", (int)textures.size());
}


void AssetManager::PreloadCustomerTextures()
{
    if (!customerTextures.empty()) {
        return;
    }

    customerTextures["normal"] = LoadTexture("ASSets/textures/customers/normal.png");
    customerTextures["rushing"] = LoadTexture("ASSets/textures/customers/rushing.png");
    customerTextures["talkative"] = LoadTexture("ASSets/textures/customers/talkative.png");
    customerTextures["angry"] = LoadTexture("ASSets/textures/customers/angry.png");
    customerTextures["confused"] = LoadTexture("ASSets/textures/customers/confused.png");

    customerTextures["shrek"] = LoadTexture("ASSets/textures/customers/shrek.png");
    customerTextures["barns_courtney"] = LoadTexture("ASSets/textures/customers/barns_courtney.png");
    customerTextures["doctor10"] = LoadTexture("ASSets/textures/customers/10th_doc.png");
    customerTextures["chloe_price"] = LoadTexture("ASSets/textures/customers/chloe.png");
    customerTextures["big_krtkus"] = LoadTexture("ASSets/textures/customers/krtkus.png");

    customerTextures["honza_tuna"] = LoadTexture("ASSets/textures/customers/tuna.png");
    customerTextures["honza_spacek"] = LoadTexture("ASSets/textures/customers/spacek.png");
    customerTextures["sugar_denny"] = LoadTexture("ASSets/textures/customers/denny.png");
    customerTextures["alastor"] = LoadTexture("ASSets/textures/customers/alastor.png");
}

Texture2D AssetManager::GetCustomerTexture(const std::string& id)
{
    auto it = customerTextures.find(id);
    if (it != customerTextures.end()) {
        return it->second;
    }

    auto fallback = customerTextures.find("normal");
    if (fallback != customerTextures.end()) {
        return fallback->second;
    }

    return Texture2D{ 0 };
}