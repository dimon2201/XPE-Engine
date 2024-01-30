cmake_minimum_required(VERSION 3.2)

project(xpe_example_window)

include(../engine/cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

file(GLOB_RECURSE SOURCE_FILES Window/*.c Window/*.cpp)

include_directories(Window)

include_engine(../engine)

add_engine_definitions()
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

link_engine(${PROJECT_NAME} ../engine)