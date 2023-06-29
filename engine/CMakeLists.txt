cmake_minimum_required(VERSION 3.2)

project(xpe_engine)

# Include CMake helpers
include(cmake_tools/CMakeLists.txt)

# Defines engine as DLL build
build_engine_dll()

# Add engine sources
file(GLOB_RECURSE SOURCE_FILES cpp/*.c cpp/*.cpp)

# Include third party dependency headers
include_directories(
        include
        ../thirdparty/glfw/include/GLFW
        ../thirdparty/glm/glm
        ../thirdparty/jsoncpp/include
        ../thirdparty/spdlog/include
)

# Create shared dynamic library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# Add precompile header for compile-time optimization
target_precompile_headers(${PROJECT_NAME} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:pch.h>")

# Link internal libraries
target_link_libraries(${PROJECT_NAME} PRIVATE gdi32 d3d11 d3dcompiler)

# Link third party libraries
replace_build_dir(engine/libs/Debug engine/libs/Debug)
link_libs(${PROJECT_NAME} libs/Debug)
replace_build_dir(engine/libs/Release engine/libs/Release)
link_libs(${PROJECT_NAME} libs/Release)