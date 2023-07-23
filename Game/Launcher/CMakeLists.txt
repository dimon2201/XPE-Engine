cmake_minimum_required(VERSION 3.2)

set(GAME_NAME Game)

project(${GAME_NAME})

include(../../engine/cmake_tools/CMakeLists.txt)
include(../Code/cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

file(GLOB_RECURSE SOURCE_FILES main.cpp)

include_engine(../../engine)
include_game(../Code)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_game_definitions()

target_precompile_headers(${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:../../../engine/include/pch.h>")

link_game(${PROJECT_NAME} ../Code ${GAME_NAME}Code)