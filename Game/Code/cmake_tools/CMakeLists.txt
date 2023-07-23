# set all config types available in engine project
macro(add_game_configs)

    # define all engine build config types
    set(CMAKE_CONFIGURATION_TYPES Debug;Release;)

    set(LIB_PATH ${CMAKE_BINARY_DIR}/lib)
    set(BIN_PATH ${CMAKE_BINARY_DIR}/bin)

    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_PATH})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_PATH})

    message(STATUS "Setting configuration types : ${CMAKE_CONFIGURATION_TYPES}")

    foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})

        message(STATUS "Configuration=${CONFIG}")

        string(TOUPPER ${CONFIG} UPPER_CONFIG)

        message(STATUS "Setting runtime output path ${BIN_PATH}/${CONFIG}")
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${UPPER_CONFIG} ${BIN_PATH}/${CONFIG})

    endforeach()

endmacro()

# add game definitions
macro(add_game_definitions)
    add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
endmacro()

# add DLL_BUILD macro to define game build as DLL
macro(build_game_dll)
    add_definitions(-DGAME_DLL_BUILD=1)
endmacro()

# include game and all it's dependencies and also defines game as DLL import
# provide path to game project
macro(include_game path)
    add_definitions(-DGAME_DLL=1)
    include_directories(
            ${path}/include
    )
endmacro()

# link target project with game
# provide your project name and path to game project
macro(link_game project path game)
    get_filename_component(dir_name ${PROJECT_BINARY_DIR} NAME)

    ## ---------------- Debug ---------------

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Debug/${game}.lib)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Debug/${game}.lib ${PROJECT_BINARY_DIR}/Debug/${game}.lib COPYONLY)
    endif()

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Debug/${game}.dll)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Debug/${game}.dll ${PROJECT_BINARY_DIR}/Debug/${game}.dll COPYONLY)
    endif()

    ## ---------------- Release ---------------

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Release/${game}.lib)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Release/${game}.lib ${PROJECT_BINARY_DIR}/Release/${game}.lib COPYONLY)
    endif()

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Release/${game}.dll)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Release/${game}.dll ${PROJECT_BINARY_DIR}/Release/${game}.dll COPYONLY)
    endif()

    message(STATUS "Linking ${project} -> ${game}.lib")
    message(STATUS "Linking ${project} -> ${game}.dll")
    target_link_libraries(${project} PUBLIC ${game})

endmacro()