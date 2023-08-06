cmake_minimum_required(VERSION 3.2)

project(xpe_loader)

# Include CMake helpers
include(../engine/cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

# Add sources
file(GLOB_RECURSE SOURCE_FILES cpp/*.c cpp/*.cpp)

# Include third party dependency headers
include_engine(../engine)
include_directories(
        include
        thirdparty/assimp/include
)

add_engine_definitions()
add_library(${PROJECT_NAME} ${SOURCE_FILES})

add_subdirectory(thirdparty/assimp)

# Link libraries
link_engine(${PROJECT_NAME} ../engine)
target_link_libraries(${PROJECT_NAME} PUBLIC assimp)