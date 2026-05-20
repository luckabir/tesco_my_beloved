#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"

class AssetManager {
public:

    static Font mainFont;
    static Music menuMusic;
    static Music gameMusic;

    static void LoadAll();
    static void UnloadAll();
};

#endif