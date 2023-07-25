cmake_minimum_required(VERSION 3.2)

project(Focus)

include(../engine/cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

file(GLOB_RECURSE SOURCE_FILES cpp/*.c cpp/*.cpp)

include_engine(../engine)
include_directories(include)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
add_engine_definitions()

link_engine(${PROJECT_NAME} ../engine)

# move resources, shaders, configs into bin folder
copy_build_dir(editor/config config)
copy_build_dir(editor/resources resources)
copy_build_dir(editor/shaders shaders)
copy_build_dir(editor/templates templates)
copy_build_dir(editor/workspace workspace)
copy_build_dir(engine engine)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    copy_build_dir(editor/config ${CONFIG}/config)
    copy_build_dir(editor/resources ${CONFIG}/resources)
    copy_build_dir(editor/shaders ${CONFIG}/shaders)
    copy_build_dir(editor/templates ${CONFIG}/templates)
    copy_build_dir(editor/workspace ${CONFIG}/workspace)
    copy_build_dir(engine ${CONFIG}/engine)
endforeach()