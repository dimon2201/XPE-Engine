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
    engine/src/core/user_input.cpp
    engine/src/core/windowing.cpp
    
    engine/src/viewer/viewer.cpp

    engine/src/gltf/gltf.cpp
)

add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/jsoncpp)

include_directories(thirdparty/glfw/include/GLFW)
include_directories(thirdparty/glm/glm)
include_directories(thirdparty/jsoncpp/include)

add_library(xpe_engine ${SOURCE_FILES})