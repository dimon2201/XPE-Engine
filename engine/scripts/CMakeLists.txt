cmake_minimum_required(VERSION 3.2)

set(ENGINE_PATH ../engine)

project(GameLib)

include(../cmake_tools/CMakeLists.txt)

build_gamelib_dll()

file(GLOB_RECURSE SOURCE_FILES ../scripts/*.c ../scripts/*.cpp)

include_directories(../scripts)
include_engine(${ENGINE_PATH})

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
add_engine_definitions()
add_gamelib_definitions()

link_engine(${PROJECT_NAME} ${ENGINE_PATH})

if (EXISTS ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.dll)
    configure_file(${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.dll ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}_Edit.dll COPYONLY)
endif()
foreach(config ${CMAKE_CONFIGURATION_TYPES})
    if (EXISTS ${CMAKE_BINARY_DIR}/bin/${CONFIG}/${PROJECT_NAME}.dll)
        configure_file(${CMAKE_BINARY_DIR}/bin/${CONFIG}/${PROJECT_NAME}.dll ${CMAKE_BINARY_DIR}/bin/${CONFIG}/${PROJECT_NAME}_Edit.dll COPYONLY)
    endif()
endforeach()