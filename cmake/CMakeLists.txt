cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(MSVC_compiler_id "MSVC")
set(CLANG_compiler_id "Clang")
set(GCC_compiler_id "GNU")

set(is_using_MSVC_compiler OFF)
set(is_using_CLANG_compiler OFF)
set(is_using_GCC_compiler OFF)

set(csg "case statement generator")
project(csg)

string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} ${MSVC_compiler_id} is_using_MSVC_compiler)
string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} ${CLANG_compiler_id} is_using_CLANG_compiler)
string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} ${GCC_compiler_id} is_using_GCC_compiler)

if(NOT (is_using_MSVC_compiler OR is_using_CLANG_compiler OR is_using_GCC_compiler))
    message(FATAL_ERROR "Compiler not supported, You are using: ${CMAKE_CXX_COMPILER_ID}")
endif()


file(GLOB_RECURSE csg_source_files "../projects/csg/*.cpp")
file(GLOB_RECURSE csg_header_files "../projects/csg/*.hpp")

file(GLOB_RECURSE csg_test_files "../projects/csg/test-files/*")

# creates macros for each hpp file include path
# very handy for name changes and file moves
foreach(HEADER_FILE ${csg_header_files})
    get_filename_component(FILE_NAME ${HEADER_FILE} NAME_WE)
    set(FILE_NAME_HPP "${FILE_NAME}_hpp")
    set(HEADER_FILE_PATH "\"${HEADER_FILE}\"")
    add_compile_definitions(-D${FILE_NAME_HPP}=${HEADER_FILE_PATH})
    
    # for file name changes in the project
    # their is a file called names.hpp that manages all file name changes in the project
    if(${FILE_NAME} STREQUAL "names")
        add_compile_definitions(-DNAMES_INCLUDE=${FILE_NAME_HPP})
    endif()
endforeach()


# creates macros for each resource (full file path)
foreach(TEST_FILE ${csg_test_files})
    # gets the name from LOL_BLOCKS_RESOURCE, LOL_BLOCKS_RESOURCE is the full path from C:
    get_filename_component(FILE_NAME ${TEST_FILE} NAME_WE)
    
    # gets the extension from LOL_BLOCKS_RESOURCE
    get_filename_component(FILE_EXT ${TEST_FILE} EXT)
    
    if(UNICODE_DEFINED AND NOT (${FILE_EXT} STREQUAL ".h" OR ${FILE_EXT} STREQUAL ".hpp"))
        # full file path in the variable LOL_BLOCKS_RESOURCE_FILE_PATH
        set(TEST_FILE_FILE_PATH "L\"${TEST_FILE}\"")
    else()
        # full file path in the variable LOL_BLOCKS_FILE_PATH
        set(TEST_FILE_FILE_PATH "\"${TEST_FILE}\"")
    endif()

    # get rid of the dot and replace it with _
    string(REPLACE "." "_" FILE_EXT ${FILE_EXT})

    # the macro name
    set(TEST_FILE_NAME "${FILE_NAME}${FILE_EXT}")
    
    # set the macro to the file path
    add_compile_definitions(-D${TEST_FILE_NAME}=${TEST_FILE_FILE_PATH})
endforeach()

add_executable(${PROJECT_NAME} ${csg_source_files} ${csg_header_files})


