# include loader and all it's dependencies
# provide path to loader project
macro(include_loader path)
    include_directories(
            ${path}/include
            ${path}/thirdparty/assimp/include
    )
endmacro()