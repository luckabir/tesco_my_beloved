#include "AssetManager.h"

Font AssetManager::mainFont = { 0 };
Music AssetManager::bgMusic = { 0 };

void AssetManager::LoadAll()
{
    mainFont = LoadFontEx("ASSets/fonts/Daydream.otf",64,nullptr,0);
    bgMusic = LoadMusicStream("ASSets/sounds/ratsong.mp3");
}

void AssetManager::UnloadAll()
{
    UnloadFont(mainFont);
    UnloadMusicStream(bgMusic);
}