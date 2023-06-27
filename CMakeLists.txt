cmake_minimum_required(VERSION 3.20)

project(XPE-Engine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

include(engine/cmake_tools/CMakeLists.txt)

# Add build targets
if(DEBUG)
    add_definitions(-DDEBUG=1)
endif()

# Add Engine
add_subdirectory(engine)

# Add Samples
add_subdirectory(samples)