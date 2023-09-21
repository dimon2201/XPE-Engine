# include loader and all it's dependencies
# provide path to loader project
macro(include_loader path)
    add_definitions(-DLOADER_DLL=1)
    add_definitions(-DASSIMP_DLL=1)
    include_directories(
            ${path}/include
            ${path}/thirdparty/Assimp142/include
            ${path}/thirdparty/Freetype/include
            ${path}/thirdparty/stbi
            ${path}/thirdparty/libsndfile/include
    )
endmacro()
# link target project with loader
# provide your project name and path to loader project
macro(link_loader project path)
    get_filename_component(dir_name ${PROJECT_BINARY_DIR} NAME)

    ## ---------------- Debug ---------------

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Debug/xpe_loader.lib)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Debug/xpe_loader.lib ${PROJECT_BINARY_DIR}/Debug/xpe_loader.lib COPYONLY)
    endif()

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Debug/xpe_loader.dll)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Debug/xpe_loader.dll ${PROJECT_BINARY_DIR}/Debug/xpe_loader.dll COPYONLY)
    endif()

    ## ---------------- Release ---------------

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Release/xpe_loader.lib)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Release/xpe_loader.lib ${PROJECT_BINARY_DIR}/Release/xpe_loader.lib COPYONLY)
    endif()

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Release/xpe_loader.dll)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Release/xpe_loader.dll ${PROJECT_BINARY_DIR}/Release/xpe_loader.dll COPYONLY)
    endif()

    message(STATUS "Linking ${project} -> xpe_loader.lib")
    message(STATUS "Linking ${project} -> xpe_loader.dll")
    target_link_libraries(${project} PUBLIC xpe_loader)

endmacro()
# add DLL_BUILD macro to define loader build as DLL
macro(build_loader_dll)
    add_definitions(-DLOADER_DLL_BUILD=1)
    add_definitions(-DASSIMP_DLL=1)
endmacro()