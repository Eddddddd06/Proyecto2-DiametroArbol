cmake_minimum_required(VERSION 3.16)
project(DiametroArbol CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Intentar usar raylib del sistema ──────────────────────────
find_package(raylib 4.0 QUIET)

if(NOT raylib_FOUND)
    message(STATUS "raylib no encontrado en el sistema — descargando con FetchContent...")
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.0
        GIT_SHALLOW    TRUE
    )
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(raylib)
endif()

add_executable(DiametroArbol
    src/main.cpp
    src/Tree.cpp
    src/Visualizer.cpp
)

target_include_directories(DiametroArbol PRIVATE src)
target_link_libraries(DiametroArbol PRIVATE raylib)

if(UNIX AND NOT APPLE)
    target_link_libraries(DiametroArbol PRIVATE m pthread dl)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
target_compile_options(DiametroArbol PRIVATE -O2 -Wall)
