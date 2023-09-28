cmake_minimum_required(VERSION 3.20)

project(XPE-Engine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3")

# Add Loader
add_subdirectory(loader)

# Add Engine
add_subdirectory(engine)

# Add Samples
add_subdirectory(samples)

# Add Editor
#add_subdirectory(editor)