/***********************************************
* File: types.hpp
*
* Purpose: Manage types throughout xxxxxxxxx
*
*
*
************************************************/

#pragma once
#include NAMES_INCLUDE
#include STL_INCLUDE_PATH
#include WINDOWS_INCLUDE_PATH


#if defined(UNICODE) || defined(_UNICODE) 

namespace xxxxxxx {
	using string = std::wstring;
	using ostringstream = std::wostringstream;
	using character = wchar_t;
	using w32_str_p = LPWSTR;			// wchar_t*
	using w32_str_cp = LPCWSTR;			// const wchar_t* (read only)
	namespace fs = std::filesystem;		// for less typing
}


#else

namespace xxxxxxx {
	using string = std::string;
	using ostringstream = std::ostringstream;
	using character = char;
	using w32_str_p = LPSTR;			// char*
	using w32_str_cp = LPCSTR;			// const char* (read only)
	namespace fs = std::filesystem;		// for less typing
}


#endif