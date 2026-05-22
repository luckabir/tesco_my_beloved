#include "AssetManager.h"

Font AssetManager::mainFont = { 0 };
Music AssetManager::menuMusic = { 0 };
Music AssetManager::gameMusic = { 0 };
Music* AssetManager::currentMusic = nullptr;

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