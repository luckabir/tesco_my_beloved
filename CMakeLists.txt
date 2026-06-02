cmake_minimum_required(VERSION 3.16)

project(tesco_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(raylib QUIET)

if (NOT raylib_FOUND)
    include(FetchContent)

    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
    )

    FetchContent_MakeAvailable(raylib)
endif()

add_executable(
    tesco_game
    main.cpp
    classes/Customer.cpp
    classes/Day.cpp
    classes/Player.cpp
    classes/Item.cpp
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

target_link_libraries(tesco_game PRIVATE raylib)

if (WIN32)
    target_link_libraries(tesco_game PRIVATE
        opengl32
        gdi32
        winmm
    )
endif()

if (UNIX AND NOT APPLE)
    target_link_libraries(tesco_game PRIVATE
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
    $<TARGET_FILE_DIR:tesco_game>/ASSets/data
)

add_custom_command(TARGET tesco_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
    $<TARGET_FILE_DIR:tesco_game>/ASSets/data/profiles
)