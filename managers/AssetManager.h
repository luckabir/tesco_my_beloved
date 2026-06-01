#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"
#include <string>
#include <vector>
#include <map>

enum MusicType
{
    MUSIC_NONE,
    MUSIC_MENU,
    MUSIC_GAME,
    MUSIC_DEPRESSION
};

enum ItemCategory {
    NORMAL,
    RESTRICTED_18,
    BAKERY,
    WEIGHED
};

struct ItemTemplate {
    std::string id;
    std::string name;
    ItemCategory category; 
    int basePrice;
    int clubcardPrice;
};

class AssetManager {
public:
    static Font mainFont;

    static Sound menuMusic;
    static Sound gameMusic;
    static Sound depressionMusic;
    static Sound scanSound;
    static Sound cardSound;
    static Texture2D clubcardTexture;
    static Texture2D cartTexture;
    static std::vector<ItemTemplate> itemDatabase; 
    static std::map<std::string, Texture2D> textures;
    static std::map<std::string, Texture2D> customerTextures;

    static void LoadAll();
    static void UnloadAll();
    static void UpdateAudio();

    static void PlayMenuMusic();
    static void PlayGameMusic();
    static void StopAllMusic();
    static void SetActiveMusic(MusicType type);

    static void LoadItemsCSV(const std::string& path);
    static ItemTemplate GetRandomItemTemplate();
    static Texture2D GetTexture(const std::string& id);
    static void PreloadItemTextures();

    static void PreloadCustomerTextures();
    static Texture2D GetCustomerTexture(const std::string& id);

    static void PlayScanSound();
    static void PlayCardSound();

private:
    static Sound* currentMusic;
};

#endif