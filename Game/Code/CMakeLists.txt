cmake_minimum_required(VERSION 3.2)

set(GAME_NAME Game)

project(${GAME_NAME}Code)

include(../../engine/cmake_tools/CMakeLists.txt)
include(cmake_tools/CMakeLists.txt)

build_game_dll()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

file(GLOB_RECURSE SOURCE_FILES cpp/*.c cpp/*.cpp)

include_engine(../../engine)
include_directories(include)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
add_engine_definitions()
add_game_definitions()

link_engine(${PROJECT_NAME} ../../engine)

# move resources, shaders and configs into bin folder
copy_build_file(${GAME_NAME}/Code/config.json config.json)
copy_build_file(${GAME_NAME}/Code/memory_config.json memory_config.json)
copy_build_file(${GAME_NAME}/Code/test_config.json test_config.json)
copy_build_dir(${GAME_NAME}/Code/resources resources)
copy_build_dir(${GAME_NAME}/Code/shaders shaders)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    copy_build_file(${GAME_NAME}/Code/config.json ${CONFIG}/config.json)
    copy_build_file(${GAME_NAME}/Code/memory_config.json ${CONFIG}/memory_config.json)
    copy_build_file(${GAME_NAME}/Code/test_config.json ${CONFIG}/test_config.json)
    copy_build_dir(${GAME_NAME}/Code/resources ${CONFIG}/resources)
    copy_build_dir(${GAME_NAME}/Code/shaders ${CONFIG}/shaders)
endforeach()