cmake_minimum_required(VERSION 3.2)

project(xpe_example_window)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

set(SOURCE_FILES_SAMPLE_WINDOW Window/main.cpp)
set(LINK_DIRECTORIES_SAMPLE_WINDOW ../build ../build/thirdparty/glfw/src)

include_directories(../thirdparty/glfw/include/GLFW ../thirdparty/glm/glm)
link_directories(xpe_example_window ${LINK_DIRECTORIES_SAMPLE_WINDOW})

add_executable(xpe_example_window ${SOURCE_FILES_SAMPLE_WINDOW})

target_link_libraries(xpe_example_window xpe_engine glfw3 gdi32 d3d11 d3dcompiler)