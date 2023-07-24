cmake_minimum_required(VERSION 3.2)

set(GAME_NAME Game)

project(${GAME_NAME})

include(../../engine/cmake_tools/CMakeLists.txt)
include(../cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

file(GLOB_RECURSE SOURCE_FILES main.cpp)

include_engine(../../engine)
include_game(../Code)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_engine_definitions()
add_game_definitions()

link_engine(${PROJECT_NAME} ../../engine)
link_game(${PROJECT_NAME} ../Code ${GAME_NAME}Code)

# move resources, shaders and configs into bin folder
copy_build_file(${GAME_NAME}/config.json config.json)
copy_build_file(${GAME_NAME}/memory_config.json memory_config.json)
copy_build_file(${GAME_NAME}/test_config.json test_config.json)
copy_build_dir(${GAME_NAME}/resources resources)
copy_build_dir(${GAME_NAME}/shaders shaders)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    copy_build_file(${GAME_NAME}/config.json ${CONFIG}/config.json)
    copy_build_file(${GAME_NAME}/memory_config.json ${CONFIG}/memory_config.json)
    copy_build_file(${GAME_NAME}/test_config.json ${CONFIG}/test_config.json)
    copy_build_dir(${GAME_NAME}/resources ${CONFIG}/resources)
    copy_build_dir(${GAME_NAME}/shaders ${CONFIG}/shaders)
endforeach()