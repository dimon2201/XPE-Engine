cmake_minimum_required(VERSION 3.2)

project(xpe_example_window)

include(../engine/cmake_tools/CMakeLists.txt)
include(../loader/cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

file(GLOB_RECURSE SOURCE_FILES Window/*.c Window/*.cpp)

include_engine(../engine)
include_loader(../loader)

add_engine_definitions()
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

link_engine(${PROJECT_NAME} ../engine)
link_loader(${PROJECT_NAME} ../loader)

# move files, shaders, configs into bin folder
copy_build_dir(samples/config config)
copy_build_dir(samples/res res)
copy_build_dir(samples/shaders shaders)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    copy_build_dir(samples/config ${CONFIG}/config)
    copy_build_dir(samples/res ${CONFIG}/res)
    copy_build_dir(samples/shaders ${CONFIG}/shaders)
endforeach()