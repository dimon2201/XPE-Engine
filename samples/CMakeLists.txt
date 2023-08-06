cmake_minimum_required(VERSION 3.2)

project(xpe_example_window)

include(../engine/cmake_tools/CMakeLists.txt)
include(../loader/cmake_tools/CMakeLists.txt)

file(GLOB_RECURSE SOURCE_FILES Window/*.c Window/*.cpp)

include_engine(../engine)
include_loader(../loader)

add_engine_definitions()
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

link_engine(${PROJECT_NAME} ../engine)
target_link_libraries(${PROJECT_NAME} PUBLIC xpe_loader)

# move files, shaders, configs into bin folder
copy_build_dir(samples/config config)
copy_build_dir(samples/resources resources)
copy_build_dir(samples/shaders shaders)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    copy_build_dir(samples/config ${CONFIG}/config)
    copy_build_dir(samples/resources ${CONFIG}/resources)
    copy_build_dir(samples/shaders ${CONFIG}/shaders)
endforeach()