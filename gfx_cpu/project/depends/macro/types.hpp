/***************************************
*  File: types.hpp
*
*  Purpose: handle unicode within namespaces
*
*  Project: core
* *************************************/

#pragma once
#include NAMES_INCLUDE
#include STL_INCLUDE_PATH
#include WINDOWS_INCLUDE_PATH




#define MAX_STRING 512

#if defined(UNICODE) || defined(_UNICODE)

// macro for handling wide strings 
#define WIDE 1

// read only string, prepends L""
#define ROS(x) L##x

#define COUT std::wcout
namespace core {
	using string = std::wstring;
	using character = wchar_t;
	using character_p = character*;
	using const_character_p = const character*;
}

#else

#define ROS(x) x
#define NARROW 1

namespace core {
	using string = std::string;
	using character = char;
	using character_p = character*;
	using const_character_p = const character*;
}


#endif

// terminal logging
#define CORE_STD_COUT 1
#define CORE_VS_OUT_WINDOW 1