cmake_minimum_required(VERSION 3.2)

project(xpe_loader)

# Include CMake helpers
include(cmake_tools/CMakeLists.txt)
include(../engine/cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

# Defines engine as DLL build
build_loader_dll()

# Add sources
file(GLOB_RECURSE SOURCE_FILES cpp/*.c cpp/*.cpp)

# Include third party dependency headers
include_engine(../engine)
include_directories(
        include
        thirdparty/Assimp/include
        thirdparty/Freetype/include
        thirdparty/stbi
)

add_engine_definitions()
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

# Link libraries
link_engine(${PROJECT_NAME} ../engine)
target_link_libraries(${PROJECT_NAME} PUBLIC ../lib/assimp-vc142-mt ../lib/freetype)

# Copy binaries and libs
configure_file(${CMAKE_SOURCE_DIR}/loader/thirdparty/Assimp/lib/Release/assimp-vc142-mt.lib ${CMAKE_BINARY_DIR}/lib/assimp-vc142-mt.lib COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/loader/thirdparty/Assimp/bin/Release/assimp-vc142-mt.dll ${CMAKE_BINARY_DIR}/bin/Debug/assimp-vc142-mt.dll COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/loader/thirdparty/Assimp/bin/Release/assimp-vc142-mt.dll ${CMAKE_BINARY_DIR}/bin/Release/assimp-vc142-mt.dll COPYONLY)

configure_file(${CMAKE_SOURCE_DIR}/loader/thirdparty/freetype2/lib/freetype.lib ${CMAKE_BINARY_DIR}/lib/freetype.lib COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/loader/thirdparty/freetype2/bin/freetype.dll ${CMAKE_BINARY_DIR}/bin/Debug/freetype.dll COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/loader/thirdparty/freetype2/bin/freetype.dll ${CMAKE_BINARY_DIR}/bin/Release/freetype.dll COPYONLY)