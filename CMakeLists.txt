cmake_minimum_required(VERSION 3.20)

project(XPE-Engine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

# Add Engine
add_subdirectory(engine)

# Add Samples
add_subdirectory(samples)

# Add Game Code
add_subdirectory(Game/Code)

# Add Game Launcher
add_subdirectory(Game/Launcher)