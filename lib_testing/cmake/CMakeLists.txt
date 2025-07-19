# 3.18 min version required
cmake_minimum_required(VERSION 3.18)

# set the name of the project
set(sdl_core "city")

# create the main project
project(${sdl_core})

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
# sdl core exe
#############################

# packages required
find_package(SDL2 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(bgfx CONFIG REQUIRED)

# get core source and header files
file(GLOB_RECURSE core_sf "../projects/*.cpp") # core_sf (core source files)
file(GLOB_RECURSE core_hf "../projects/*.hpp") # core_sf (core header files)


# ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/../libs/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# core exe
add_executable(${sdl_core} ${core_sf} ${core_hf} ${IMGUI_SOURCES})

target_link_libraries(${sdl_core}
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        assimp::assimp
        glm::glm
        spdlog::spdlog
        bgfx::bx 
        bgfx::bgfx 
        bgfx::bimg 
        bgfx::bimg_decode
)


# core header file names
# creates macros for each hpp file include path
# very handy for name changes and file moves
foreach(HEADER_FILE ${core_hf})
    get_filename_component(FILE_NAME ${HEADER_FILE} NAME_WE)
    set(FILE_NAME_HPP "CORE_${FILE_NAME}_hpp")
    set(HEADER_FILE_PATH "\"${HEADER_FILE}\"")
    target_compile_definitions(${sdl_core} PRIVATE -D${FILE_NAME_HPP}=${HEADER_FILE_PATH})
    
    # for file name changes in the project
    # there is a file called names.hpp that manages all file name changes in the project
    if(${FILE_NAME} STREQUAL "names")
        target_compile_definitions(${sdl_core} PRIVATE -DCORE_NAMES_INCLUDE=${FILE_NAME_HPP})
    endif()
endforeach()

#############################
# End of sdl core exe
#############################
