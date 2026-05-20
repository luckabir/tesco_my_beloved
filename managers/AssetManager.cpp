#include "AssetManager.h"

Font AssetManager::mainFont = { 0 };
Music AssetManager::gameMusic = { 0 };
Music AssetManager::menuMusic = { 0 };


void AssetManager::LoadAll()
{
    mainFont = LoadFontEx("ASSets/fonts/Daydream.otf",64,nullptr,0);
    menuMusic = LoadMusicStream("ASSets/sounds/menu.mp3");
    gameMusic = LoadMusicStream("ASSets/sounds/game.mp3");

    menuMusic.looping = true;
    gameMusic.looping = true;

    PlayMusicStream(menuMusic);
    PlayMusicStream(gameMusic);
}

void AssetManager::UnloadAll()
{
    UnloadFont(mainFont);
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(gameMusic);
}