cmake_minimum_required(VERSION 3.21 FATAL_ERROR)
project(Starfield_Battle)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Starfield_Battle
    WIN32
    ArmedSprite.cpp
    ArmedSprite.h
    Colors.cpp
    Colors.h
    DDManager.cpp
    DDManager.h
    DIManager.cpp
    DIManager.h
    DXAManager.cpp
    DXAManager.h
    Engine.h
    FXManager.cpp
    FXManager.h
    GameManager.cpp
    GameManager.h
    List.cpp
    List.h
    Main.cpp
    Menu.cpp
    Menu.h
    RS2.rc
    Sprites.cpp
    Sprites.h
    Starfield.cpp
    Starfield.h
)
target_include_directories(Starfield_Battle
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    DirectX
)
target_link_libraries(Starfield_Battle
    PRIVATE
    dinput8.lib
    ddraw.lib
    dxguid.lib
)
target_compile_definitions(Starfield_Battle PRIVATE -DINITGUID)

set_target_properties(
    Starfield_Battle PROPERTIES
    OUTPUT_NAME "Starfield Battle"
)
