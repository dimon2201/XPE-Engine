cmake_minimum_required(VERSION 3.2)

project(xpe_example_window)

include(../engine/cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

file(GLOB_RECURSE SOURCE_FILES Window/*.c Window/*.cpp)

include_engine(../engine)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_engine_definitions()

link_engine(${PROJECT_NAME} ../engine)

# move files, shaders, configs into bin folder
copy_build_file(samples/config.json config.json)
copy_build_file(samples/test_config.json test_config.json)
copy_build_dir(samples/resources bin/resources)
copy_build_dir(samples/shaders bin/shaders)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    copy_build_dir(samples/resources bin/${CONFIG}/resources)
    copy_build_dir(samples/shaders bin/${CONFIG}/shaders)
endforeach()