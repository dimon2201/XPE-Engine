cmake_minimum_required(VERSION 3.2)

project(xpe_loader)

# Include CMake helpers
include(engine/cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

# Add sources
file(GLOB_RECURSE SOURCE_FILES cpp/*.c cpp/*.cpp)

# Include third party dependency headers
include_engine()
include_directories(
        include
        thirdparty/glfw/include/GLFW
        thirdparty/glm/glm
        thirdparty/json/include
        thirdparty/spdlog/include
        thirdparty/stbi
        thirdparty/freetype2/include
        thirdparty/assimp/include
)

# Create shared dynamic library
add_engine_definitions()
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# Add precompile header for compile-CurrentTime optimization
target_precompile_headers(${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:pch.h>")

add_subdirectory(thirdparty/spdlog)
add_subdirectory(thirdparty/glfw)
add_subdirectory(thirdparty/glm)
add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/freetype2)
add_subdirectory(thirdparty/assimp)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE gdi32 d3d11 d3dcompiler)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog glfw glm freetype nlohmann_json assimp)

# Move shaders into bin folder
copy_engine_shaders(engine)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    copy_engine_shaders_to_config(engine ${CONFIG})
endforeach()