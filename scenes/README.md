# HOW TO COMPILE

## 1. Compile

```bash
g++ main.cpp \
    classes/Customer.cpp classes/Day.cpp classes/Player.cpp \
    scenes/game_main.cpp \
    scenes/game/scene_calendar.cpp \
    scenes/game/scene_game.cpp \
    scenes/game/scene_stats.cpp \
    scenes/scene_menu.cpp \
    scenes/scene_settings.cpp \
    scenes/scene_score.cpp \
    scenes/scene_profile.cpp \
    scenes/scene_saves.cpp \
    scenes/scene_intro.cpp \
    structures/Settings.cpp \
    structures/Profile.cpp \
    managers/InputManager.cpp \
    managers/AssetManager.cpp \
    -o tesco_game \
    -I. \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 \
    -g -fsanitize=address -fno-omit-frame-pointer
```

## 2. Run

```bash
./tesco_game
```

## 3. Use realistic settings

## 4. Find real job