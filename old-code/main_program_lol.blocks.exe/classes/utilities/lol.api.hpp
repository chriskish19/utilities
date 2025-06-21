/****************************************************************
	Header: lol.api.hpp

	Purpose: helper functions 

****************************************************************/


#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// errors
#include "main_program_lol.blocks.exe/errors/lol.codes.hpp"

// globals
#include "main_program_lol.blocks.exe/global/globals.hpp"


namespace utilities {
	// string conversions, if max_string_buffer is exceeded
	// new is used to allocate a heap buffer to compensate
	const size_t max_string_buffer = MAX_STRING;
	std::wstring to_wide_string(const char* narrow);
	std::wstring to_wide_string(const std::string& narrow);
	std::string to_narrow_string(const wchar_t* wide);
	std::string to_narrow_string(const std::wstring& wide);

	// string conversions with error codes, if max_string_buffer is exceeded
	// new is used to allocate a heap buffer to compensate
	std::wstring to_wide_string(const char* narrow,errors::codes* code_p);
	std::wstring to_wide_string(const std::string& narrow, errors::codes* code_p);
	std::string to_narrow_string(const wchar_t* wide, errors::codes* code_p);
	std::string to_narrow_string(const std::wstring& wide, errors::codes* code_p);


	// checks the menu pointer from CreateMenu()
	errors::win32_codes win32_menu_check(HMENU p_menu,const string& location = errors::get_location());

	// checks the function AppendMenu() return value and handles the error
	errors::win32_codes win32_append_menu_check(BOOL code, const string& location = errors::get_location());
}