cmake_minimum_required(VERSION 3.2)

set(GAME_NAME Game)
set(ENGINE_PATH ../engine)

project(${GAME_NAME}Code)

include(${ENGINE_PATH}/cmake_tools/CMakeLists.txt)
include(../cmake_tools/CMakeLists.txt)

build_game_dll()

file(GLOB_RECURSE SOURCE_FILES cpp/*.c cpp/*.cpp)

include_directories(include)
include_engine(${ENGINE_PATH})

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
add_engine_definitions()
add_game_definitions()

link_engine(${PROJECT_NAME} ${ENGINE_PATH})

if (EXISTS ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.dll)
    configure_file(${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.dll ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}_Edit.dll COPYONLY)
endif()
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    if (EXISTS ${CMAKE_BINARY_DIR}/bin/${CONFIG}/${PROJECT_NAME}.dll)
        configure_file(${CMAKE_BINARY_DIR}/bin/${CONFIG}/${PROJECT_NAME}.dll ${CMAKE_BINARY_DIR}/bin/${CONFIG}/${PROJECT_NAME}_Edit.dll COPYONLY)
    endif()
endforeach()