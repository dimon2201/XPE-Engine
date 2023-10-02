cmake_minimum_required(VERSION 3.2)

project(xpe_engine)

# Include CMake helpers
include(cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -O3")

# Defines engine as DLL build
build_engine_dll()

# Add engine sources
file(GLOB_RECURSE SOURCE_FILES cpp/*.c cpp/*.cpp)

# Include third party dependency headers
include_directories(
        include
        thirdparty/glfw/include/GLFW
        thirdparty/glm/glm
        thirdparty/json/include
        thirdparty/spdlog/include
        thirdparty/stbi
        thirdparty/openal-soft/include/AL
        thirdparty/libsndfile/include
        thirdparty/PhysX/physx/include
        thirdparty/PhysX/blast/include
        thirdparty/PhysX/flow/include
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
add_subdirectory(thirdparty/openal-soft)
add_subdirectory(thirdparty/libsndfile)

# Copy binaries and libs
configure_file(${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/release/PhysX_64.lib ${CMAKE_BINARY_DIR}/lib/PhysX_64.lib COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/debug/PhysX_64.dll ${CMAKE_BINARY_DIR}/bin/Debug/PhysX_64.dll COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/release/PhysX_64.dll ${CMAKE_BINARY_DIR}/bin/Release/PhysX_64.dll COPYONLY)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE gdi32 d3d11 d3dcompiler )
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog glfw glm nlohmann_json OpenAL sndfile)
target_link_libraries(${PROJECT_NAME} PUBLIC ../lib/PhysX_64)

# Move shaders into bin folder
copy_engine_shaders(engine)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    copy_engine_shaders_to_config(engine ${CONFIG})
endforeach()