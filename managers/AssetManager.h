#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"

class AssetManager {
public:

    static Font mainFont;
    static Music bgMusic;

    static void LoadAll();
    static void UnloadAll();
};

#endif