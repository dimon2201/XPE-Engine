cmake_minimum_required(VERSION 3.2)

project(xpe_engine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

set(SOURCE_FILES
    engine/src/core/app.cpp
    engine/src/core/memory_pool.cpp
    engine/src/core/types.cpp
    engine/src/core/windowing.cpp
)

add_subdirectory(thirdparty/glfw)

include_directories(thirdparty/glfw/include/GLFW)
include_directories(thirdparty/glm/glm)

add_library(xpe_engine ${SOURCE_FILES})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/build/libxpe_engine.a ${CMAKE_CURRENT_SOURCE_DIR}/samples/Window/libxpe_engine.a COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/build/thirdparty/glfw/src/libglfw3.a ${CMAKE_CURRENT_SOURCE_DIR}/samples/Window/libglfw3.a COPYONLY)