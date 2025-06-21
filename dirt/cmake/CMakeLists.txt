# 3.18 min version required
cmake_minimum_required(VERSION 3.18)

# set the name of the project
set(dirt_core "dirt.core")

# create the main project
project(${dirt_core})

# using c++ 20 required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# check compiler version
set(MSVC_compiler_id "MSVC")
set(CLANG_compiler_id "Clang")
set(GCC_compiler_id "GNU")
set(is_using_MSVC_compiler OFF)
set(is_using_CLANG_compiler OFF)
set(is_using_GCC_compiler OFF)
string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} ${MSVC_compiler_id} is_using_MSVC_compiler)
string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} ${CLANG_compiler_id} is_using_CLANG_compiler)
string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} ${GCC_compiler_id} is_using_GCC_compiler)

# error on wrong compiler
if(NOT (is_using_MSVC_compiler OR is_using_CLANG_compiler OR is_using_GCC_compiler))
    message(FATAL_ERROR "Compiler not supported, You are using: ${CMAKE_CXX_COMPILER_ID}")
endif()

#############################
# dirt core exe
#############################

# get core source and header files
file(GLOB_RECURSE dt_core_sf "../projects/dirt.core/*.cpp") # dt_core_sf (core source files)
file(GLOB_RECURSE dt_core_hf "../projects/dirt.core/*.hpp") # dt_core_sf (core header files)

# core exe
add_executable(${dirt_core} ${dt_core_sf} ${dt_core_hf})

# core header file names
# creates macros for each hpp file include path
# very handy for name changes and file moves
foreach(HEADER_FILE ${dt_core_hf})
    get_filename_component(FILE_NAME ${HEADER_FILE} NAME_WE)
    set(FILE_NAME_HPP "CORE_${FILE_NAME}_hpp")
    set(HEADER_FILE_PATH "\"${HEADER_FILE}\"")
    target_compile_definitions(${dirt_core} PRIVATE -D${FILE_NAME_HPP}=${HEADER_FILE_PATH})
    
    # for file name changes in the project
    # their is a file called names.hpp that manages all file name changes in the project
    if(${FILE_NAME} STREQUAL "names")
        target_compile_definitions(${dirt_core} PRIVATE -DCORE_NAMES_INCLUDE=${FILE_NAME_HPP})
    endif()
endforeach()
#############################
# End of dirt core exe
#############################
