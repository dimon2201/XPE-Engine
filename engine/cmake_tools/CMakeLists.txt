# set all config types available in engine project
macro(add_engine_configs)

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

# add engine definitions
macro(add_engine_definitions)
    add_engine_configs()
    add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
    if(DEBUG)
        add_definitions(-DDEBUG=1)
    endif(DEBUG)
endmacro()

# copying target dir to destination dir. If destination does not exist, create it.
macro(copy_dir dir1 dir2)
    set(srcDir ${dir1})
    set(destDir ${dir2})

    if (NOT IS_DIRECTORY ${destDir})
        message(STATUS "Creating directory ${destDir}")
        make_directory(${destDir})
    endif()

    message(STATUS "Search files in directory ${srcDir}")
    file(GLOB_RECURSE templateFiles RELATIVE ${srcDir} ${srcDir}/*)

    foreach(templateFile ${templateFiles})
        message(STATUS "Copying files from directory ${srcDir} into ${destDir}")
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Target file path ${srcTemplatePath}")
            message(STATUS "Destination file path ${destDir}/${templateFile}")
            configure_file(${srcTemplatePath} ${destDir}/${templateFile} COPYONLY)
        endif()
    endforeach()

endmacro()

# copying source directory into binary directory
macro(copy_build_dir dir1 dir2)
    copy_dir(${CMAKE_SOURCE_DIR}/${dir1} ${CMAKE_BINARY_DIR}/bin/${dir2})
endmacro()

# copying source file into binary file folder
macro(copy_build_file f1 f2)
    configure_file(${CMAKE_SOURCE_DIR}/${f1} ${CMAKE_BINARY_DIR}/bin/${f2} COPYONLY)
endmacro()

# delete directory
macro(delete_dir dir)
    set(buildDir ${dir})
    message(STATUS "Deleting build directory ${buildDir}")
    file(REMOVE_RECURSE ${buildDir})
endmacro()

# delete binary directory
macro(delete_build_dir dir)
    delete_dir(${CMAKE_BINARY_DIR}/${dir})
endmacro()

# delete destination dir and copy target dir into destination dir.
macro(replace_dir targetDir destDir)
    delete_dir(${destDir})
    copy_dir(${targetDir} ${destDir})
endmacro()

macro(replace_build_dir targetDir destDir)
    delete_build_dir(${destDir})
    copy_build_dir(${targetDir} ${destDir})
endmacro()

# replace file
# provide "from" file path and "to" file path
macro(replace_file srcFile destFile)
    if (EXISTS ${srcFile})
        file(REMOVE_RECURSE ${destFile})
        configure_file(${srcFile} ${destFile} COPYONLY)
    endif()
endmacro()

# replace file from sources into build
macro(replace_build_file file)
    replace_file(${PROJECT_SOURCE_DIR}/${file} ${PROJECT_BINARY_DIR}/${file})
endmacro()

# dynamic linking of lib directory
# provide your project name and libs directory path
macro(dynamic_link project src_dir)
    message(STATUS "Dynamically linking files from directory ${src_dir}")
    file(GLOB libs ${src_dir}/*.lib)
    file(GLOB dlls ${src_dir}/*.dll)

    foreach(src_filepath ${libs})
        get_filename_component(src_filename ${src_filepath} NAME)
        set(src ${src_dir}/${src_filename})
        set(dest ${PROJECT_BINARY_DIR}/${src_filename})
        message(STATUS "Target file path ${src}")
        message(STATUS "Destination file path ${dest}")
        configure_file(
                ${src}
                ${dest}
                COPYONLY
        )
        string(REPLACE ".lib" "" src_libname ${src_filename})
        target_link_libraries(${project} PUBLIC ${src_libname})
    endforeach(src_filepath)

    foreach(src_filepath ${dlls})
        get_filename_component(src_filename ${src_filepath} NAME)
        set(src ${src_dir}/${src_filename})
        set(dest ${PROJECT_BINARY_DIR}/${src_filename})
        message(STATUS "Target file path ${src}")
        message(STATUS "Destination file path ${dest}")
        configure_file(
                ${src}
                ${dest}
                COPYONLY
        )
        string(REPLACE ".dll" "" src_libname ${src_filename})
        target_link_libraries(${project} PUBLIC ${src_libname})
    endforeach()

endmacro()

# link .lib files from directory
macro(link_libs project dir)

        message(STATUS "Linking .lib files from directory ${PROJECT_BINARY_DIR}/${dir}")
        file(GLOB libs ${PROJECT_BINARY_DIR}/${dir}/*.lib)

        foreach(lib_path ${libs})
            get_filename_component(lib_name ${lib_path} NAME)
            string(REPLACE ".lib" "" lib ${lib_name})
            set(lib_relative_path ${dir}/${lib})
            message(STATUS "Linking ${project} -> ${lib_relative_path}.lib")
            target_link_libraries(${PROJECT_NAME} PRIVATE ${lib_relative_path})
        endforeach()

endmacro()

# add DLL_BUILD macro to define engine build as DLL
macro(build_engine_dll)
    add_definitions(-DENGINE_DLL_BUILD=1)
endmacro()

# include engine and all it's dependencies and also defines engine as DLL import
# provide path to engine project
macro(include_engine path)
    add_definitions(-DENGINE_DLL=1)
    include_directories(
            ${path}/include
            ${path}/thirdparty/spdlog/include
            ${path}/thirdparty/glfw/include
            ${path}/thirdparty/glm/glm
            ${path}/thirdparty/json/include
    )
endmacro()

# link target project with engine and also will add precompiled header to your project
# provide your project name and path to engine project
macro(link_engine project path)
    target_precompile_headers(${project} PRIVATE "$<$<COMPILE_LANGUAGE:CXX>:../${path}/include/pch.h>")
    get_filename_component(dir_name ${PROJECT_BINARY_DIR} NAME)

    ## ---------------- Debug ---------------

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Debug/xpe_engine.lib)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Debug/xpe_engine.lib ${PROJECT_BINARY_DIR}/Debug/xpe_engine.lib COPYONLY)
    endif()

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Debug/xpe_engine.dll)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Debug/xpe_engine.dll ${PROJECT_BINARY_DIR}/Debug/xpe_engine.dll COPYONLY)
    endif()

    ## ---------------- Release ---------------

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Release/xpe_engine.lib)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Release/xpe_engine.lib ${PROJECT_BINARY_DIR}/Release/xpe_engine.lib COPYONLY)
    endif()

    if (EXISTS ${PROJECT_BINARY_DIR}/${path}/Release/xpe_engine.dll)
        configure_file(${PROJECT_BINARY_DIR}/${path}/Release/xpe_engine.dll ${PROJECT_BINARY_DIR}/Release/xpe_engine.dll COPYONLY)
    endif()

    message(STATUS "Linking ${project} -> xpe_engine.lib")
    message(STATUS "Linking ${project} -> xpe_engine.dll")
    target_link_libraries(${project} PUBLIC xpe_engine)

endmacro()

macro(copy_engine_shaders path)
    copy_build_dir(${path}/engine_shaders engine_shaders)
endmacro()

macro(copy_engine_shaders_to_config path config)
    copy_build_dir(${path}/engine_shaders ${config}/engine_shaders)
endmacro()