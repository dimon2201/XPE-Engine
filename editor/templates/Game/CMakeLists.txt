cmake_minimum_required(VERSION 3.20)

project(Game-Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

# Add Engine
add_subdirectory(engine)

# Add Code
add_subdirectory(Code)

# Add Launcher
add_subdirectory(Launcher)