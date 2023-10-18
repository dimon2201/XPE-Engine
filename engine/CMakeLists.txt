cmake_minimum_required(VERSION 3.2)

project(xpe_engine)

# Include CMake helpers
include(cmake_tools/CMakeLists.txt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3")

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
        thirdparty/openal-soft/include/AL
        thirdparty/libsndfile/include
        thirdparty/PhysX/physx/include
        thirdparty/PhysX/blast/include
        thirdparty/PhysX/flow/include
        thirdparty/stbi
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
add_subdirectory(thirdparty/PhysX/physx/compiler/public)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE gdi32 d3d11 d3dcompiler)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog glfw glm nlohmann_json OpenAL sndfile)

# Link PhysX SDK
target_link_libraries(${PROJECT_NAME} PUBLIC
        PhysX
        PhysXCommon
        PhysXFoundation
        PhysXCooking
        PhysXVehicle
        PhysXCharacterKinematic
)

# Copy PhysX binaries
if(EXISTS ${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/debug/freeglutd.dll)
    configure_file(${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/debug/freeglutd.dll ${CMAKE_BINARY_DIR}/bin/debug/freeglutd.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/release/freeglut.dll)
    configure_file(${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/release/freeglut.dll ${CMAKE_BINARY_DIR}/bin/release/freeglut.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/debug/PhysXDevice64.dll)
    configure_file(${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/debug/PhysXDevice64.dll ${CMAKE_BINARY_DIR}/bin/debug/PhysXDevice64.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/release/PhysXDevice64.dll)
    configure_file(${CMAKE_SOURCE_DIR}/engine/thirdparty/binaries/release/PhysXDevice64.dll ${CMAKE_BINARY_DIR}/bin/release/PhysXDevice64.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/debug/PhysX.dll)
    configure_file(${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/debug/PhysX.dll ${CMAKE_BINARY_DIR}/bin/Debug/PhysX.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/release/PhysX.dll)
    configure_file(${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/release/PhysX.dll ${CMAKE_BINARY_DIR}/bin/Release/PhysX.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/debug/PhysXCommon.dll)
    configure_file(${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/debug/PhysXCommon.dll ${CMAKE_BINARY_DIR}/bin/Debug/PhysXCommon.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/release/PhysXCommon.dll)
    configure_file(${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/release/PhysXCommon.dll ${CMAKE_BINARY_DIR}/bin/Release/PhysXCommon.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/debug/PhysXCooking.dll)
    configure_file(${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/debug/PhysXCooking.dll ${CMAKE_BINARY_DIR}/bin/Release/PhysXCooking.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/release/PhysXCooking.dll)
    configure_file(${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/release/PhysXCooking.dll ${CMAKE_BINARY_DIR}/bin/Release/PhysXCooking.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/debug/PhysXFoundation.dll)
    configure_file(${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/debug/PhysXFoundation.dll ${CMAKE_BINARY_DIR}/bin/Debug/PhysXFoundation.dll COPYONLY)
endif()

if(EXISTS ${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/release/PhysXFoundation.dll)
    configure_file(${CMAKE_BINARY_DIR}/lib/bin/win.x86_64.vc143.md/release/PhysXFoundation.dll ${CMAKE_BINARY_DIR}/bin/Release/PhysXFoundation.dll COPYONLY)
endif()

# Move shaders into bin folder
copy_engine_shaders(engine)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    copy_engine_shaders_to_config(engine ${CONFIG})
endforeach()