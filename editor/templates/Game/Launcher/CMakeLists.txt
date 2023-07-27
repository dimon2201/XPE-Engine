cmake_minimum_required(VERSION 3.2)

set(GAME_NAME Game)
set(ENGINE_PATH ../engine)

project(${GAME_NAME})

include(${ENGINE_PATH}/cmake_tools/CMakeLists.txt)
include(../cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

file(GLOB_RECURSE SOURCE_FILES main.cpp)

include_engine(${ENGINE_PATH})
include_game(../Code)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_engine_definitions()
add_game_definitions()

link_engine(${PROJECT_NAME} ${ENGINE_PATH})
link_game(${PROJECT_NAME} ../Code ${GAME_NAME}Code)

# move resources, shaders and configs into bin folder
copy_build_dir(config config)
copy_build_dir(resources resources)
copy_build_dir(shaders shaders)
copy_engine_shaders(${ENGINE_PATH})
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    copy_build_dir(config ${CONFIG}/config)
    copy_build_dir(resources ${CONFIG}/resources)
    copy_build_dir(shaders ${CONFIG}/shaders)
    copy_engine_shaders_to_config(${ENGINE_PATH} ${CONFIG})
endforeach()