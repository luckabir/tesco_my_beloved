cmake_minimum_required(VERSION 3.16)

project(tesco_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tesco_game
    main.cpp

    classes/Customer.cpp
    classes/Day.cpp
    classes/Player.cpp
    classes/items/Item.cpp

    managers/CustomerManager.cpp
    managers/InputManager.cpp
    managers/AssetManager.cpp

    scenes/game_main.cpp
    scenes/game/scene_calendar.cpp
    scenes/game/scene_game.cpp
    scenes/game/scene_stats.cpp

    scenes/scene_menu.cpp
    scenes/scene_settings.cpp
    scenes/scene_score.cpp
    scenes/scene_profile.cpp
    scenes/scene_intro.cpp
    scenes/scene_controls.cpp

    structures/Settings.cpp
    structures/Profile.cpp
)

target_include_directories(tesco_game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if (WIN32)
    target_link_libraries(tesco_game PRIVATE
        raylib
        opengl32
        gdi32
        winmm
    )
endif()

if (UNIX AND NOT APPLE)
    target_link_libraries(tesco_game PRIVATE
        raylib
        GL
        m
        pthread
        dl
        rt
        X11
    )
endif()

add_custom_command(TARGET tesco_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/ASSets
    $<TARGET_FILE_DIR:tesco_game>/ASSets
)

add_custom_command(TARGET tesco_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:tesco_game>/profiles
)