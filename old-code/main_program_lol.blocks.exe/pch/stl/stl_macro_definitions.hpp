/****************************************************************
	Header: stl_macro_definitions.hpp

	Purpose: includes stl headers that match with the c++ version
			 defined in cmake. Include this header when using STL.

****************************************************************/

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
#error "ALL_STL_INCLUDES.hpp only supports 3 compiler vendors MSVC, GCC and Clang"
#endif

#if ((GENERAL_MACRO_CPP_LANG_VERSION < 201402L) || (GENERAL_MACRO_CPP_LANG_VERSION > 202302L))
#define CPP_LANG_VERSION_NOT_SUPPORTED 1
#error "ALL_STL_INCLUDES.hpp only supports c++ 14 to 23"
#endif


#if GENERAL_MACRO_CPP_LANG_VERSION == 201402L
// C++14
#define CPP_14 2014
#include "stl_includes.hpp"
#elif GENERAL_MACRO_CPP_LANG_VERSION == 201703L
// C++17
#define CPP_17 2017
#include "stl_includes.hpp"
#elif GENERAL_MACRO_CPP_LANG_VERSION == 202002L
// C++20
#define CPP_20 2020
#include "stl_includes.hpp"
#elif GENERAL_MACRO_CPP_LANG_VERSION == 202302L
// C++23
#define CPP_23 2023
#include "stl_includes.hpp"
#endif