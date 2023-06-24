cmake_minimum_required(VERSION 3.2)

project(xpe_engine)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

set(SOURCE_FILES
    engine/src/core/app.cpp
    engine/src/core/canvas.cpp
    engine/src/core/d3d11_context.cpp
    engine/src/core/memory_pool.cpp
    engine/src/core/rendering.cpp
    engine/src/core/types.cpp
    engine/src/core/windowing.cpp
)

add_subdirectory(thirdparty/glfw)

include_directories(thirdparty/glfw/include/GLFW)
include_directories(thirdparty/glm/glm)

add_library(xpe_engine ${SOURCE_FILES})