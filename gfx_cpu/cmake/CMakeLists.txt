# Foundation for building programs with direct x 11 graphics
# Only MSVC, Clang and GNU compilers are supported
# Only c++ 20 is supported
# Works best using an ide that supports folder viewing (no ide specific solution files)


cmake_minimum_required(VERSION 3.18)

# main project exe
set(foundation "gfx_cpu.core")

# create main project
project(${foundation})

# We only support c++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# We only support MSVC, Clang, and GNU compilers.
set(MSVC_compiler_id "MSVC")
set(CLANG_compiler_id "Clang")
set(GCC_compiler_id "GNU")

set(is_using_MSVC_compiler OFF)
set(is_using_CLANG_compiler OFF)
set(is_using_GCC_compiler OFF)

string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} ${MSVC_compiler_id} is_using_MSVC_compiler)
string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} ${CLANG_compiler_id} is_using_CLANG_compiler)
string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} ${GCC_compiler_id} is_using_GCC_compiler)

if(NOT (is_using_MSVC_compiler OR is_using_CLANG_compiler OR is_using_GCC_compiler))
    message(FATAL_ERROR "Compiler not supported, You are using: ${CMAKE_CXX_COMPILER_ID}")
endif()

# switch for unicode
set(UNICODE_DEFINED ON)

if(UNICODE_DEFINED)
    # define UNICODE to the compiler
    add_compile_definitions(-DUNICODE -D_UNICODE)
endif()

# directx toolkit
find_package(directxtk CONFIG REQUIRED)

# main core project
file(GLOB_RECURSE core_source_files "../project/*.cpp")
file(GLOB_RECURSE core_header_files "../project/*.hpp")
file(GLOB_RECURSE core_resources "../project/resources/*")



# creates macros for each hpp file include path
# very handy for name changes and file moves
foreach(HEADER_FILE ${core_header_files})
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
foreach(CORE_RESOURCE ${core_resources})
    # gets the name from CORE_RESOURCE, CORE_RESOURCE is the full path from C:
    get_filename_component(FILE_NAME ${CORE_RESOURCE} NAME_WE)
    
    # gets the extension from CORE_RESOURCE
    get_filename_component(FILE_EXT ${CORE_RESOURCE} EXT)
    
    if(UNICODE_DEFINED AND NOT (${FILE_EXT} STREQUAL ".h" OR ${FILE_EXT} STREQUAL ".hpp"))
        # full file path in the variable CORE_RESOURCE_FILE_PATH
        set(CORE_RESOURCE_FILE_PATH "L\"${CORE_RESOURCE}\"")
    else()
        # full file path in the variable CORE_RESOURCE_FILE_PATH
        set(CORE_RESOURCE_FILE_PATH "\"${CORE_RESOURCE}\"")
    endif()

    # get rid of the dot and replace it with _
    string(REPLACE "." "_" FILE_EXT ${FILE_EXT})

    # the macro name
    set(CORE_RESOURCE_NAME "${FILE_NAME}${FILE_EXT}")
    
    # set the macro to the file path
    add_compile_definitions(-D${CORE_RESOURCE_NAME}=${CORE_RESOURCE_FILE_PATH})
endforeach()



add_executable(${foundation} ${core_source_files} ${core_header_files})

# Link required Win32 libraries
target_link_libraries(${foundation} PUBLIC
    user32
    kernel32
    gdi32
    winmm
    comdlg32
    advapi32
    shell32
    ole32
    oleaut32
    d3d11
    dxgi
    d3dcompiler
    Microsoft::DirectXTK
    runtimeobject
)


