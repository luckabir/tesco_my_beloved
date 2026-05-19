# HOW TO COMPILE

1. copy this
    g++ main.cpp \
    classes/Customer.cpp classes/Day.cpp classes/Item.cpp classes/Player.cpp \
    scenes/game_main.cpp scenes/menu.cpp scenes/scene_settings.cpp scenes/scene_score.cpp scenes/scene_profile.cpp scenes/scene_saves.cpp \
    structures/Image.c structures/Settings.cpp structures/Sound.cpp structures/InputManager.cpp \
    -o tesco_game -I. -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

2. run it
    ./tesco_game
