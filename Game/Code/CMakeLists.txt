cmake_minimum_required(VERSION 3.2)

set(GAME_NAME Game)

project(${GAME_NAME}Code)

include(../../engine/cmake_tools/CMakeLists.txt)
include(../cmake_tools/CMakeLists.txt)

build_game_dll()

file(GLOB_RECURSE SOURCE_FILES cpp/*.c cpp/*.cpp)

include_directories(include)
include_engine(../../engine)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
add_engine_definitions()
add_game_definitions()

link_engine(${PROJECT_NAME} ../../engine)