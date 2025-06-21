/***********************************************
* File: stl.hpp
*
* Purpose: includes all standard library headers
*
* Project: core
*
************************************************/

#ifndef CORE_STL_HPP_GUARD
#define CORE_STL_HPP_GUARD

#include CORE_NAMES_INCLUDE


#if defined(_MSVC_LANG)
#define USING_MSVC_CPP_LANG_VERSION _MSVC_LANG
#define GENERAL_MACRO_CPP_LANG_VERSION _MSVC_LANG
#elif defined(__clang__)
#define USING_CLANG_CPP_LANG_VERSION __cplusplus
#define GENERAL_MACRO_CPP_LANG_VERSION __cplusplus
#elif defined(__GNUC__)
#define USING_GCC_CPP_LANG_VERSION __cplusplus
#define GENERAL_MACRO_CPP_LANG_VERSION __cplusplus
#else
#define COMPILER_NOT_SUPPORTED 1
#error "ALL_STL_INCLUDE_PATHS.hpp only supports 3 compiler vendors MSVC, GCC and Clang"
#endif

#if ((GENERAL_MACRO_CPP_LANG_VERSION < 201402L) || (GENERAL_MACRO_CPP_LANG_VERSION > 202302L))
#define CPP_LANG_VERSION_NOT_SUPPORTED 1
#error "ALL_STL_INCLUDE_PATHS.hpp only supports c++ 14 to 23"
#endif


#if GENERAL_MACRO_CPP_LANG_VERSION == 201402L
// C++14
#define CPP_14 2014
#include CORE_STL_PRIVATE_INCLUDE_PATH
#elif GENERAL_MACRO_CPP_LANG_VERSION == 201703L
// C++17
#define CPP_17 2017
#include CORE_STL_PRIVATE_INCLUDE_PATH
#elif GENERAL_MACRO_CPP_LANG_VERSION == 202002L
// C++20
#define CPP_20 2020
#include CORE_STL_PRIVATE_INCLUDE_PATH
#elif GENERAL_MACRO_CPP_LANG_VERSION == 202302L
// C++23
#define CPP_23 2023
#include CORE_STL_PRIVATE_INCLUDE_PATH
#endif


#endif // HEADER GUARD END