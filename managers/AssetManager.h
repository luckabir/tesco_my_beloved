#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"

enum MusicType
{
    MUSIC_NONE,
    MUSIC_MENU,
    MUSIC_GAME
};

class AssetManager {

    public:
        static Font mainFont;
        static Music menuMusic;
        static Music gameMusic;


        static void LoadAll();
        static void UnloadAll();
        static void UpdateAudio();
        static void PlayMenuMusic();
        static void PlayGameMusic();
        static void StopAllMusic();
        static void SetActiveMusic(MusicType type);

    private:
        static Music* currentMusic;
};



#endif


