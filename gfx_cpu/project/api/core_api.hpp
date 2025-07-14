/***********************************************
* File: core_api.hpp
*
* Purpose: helper functions
*
* Project: core
*
************************************************/


#pragma once
#include NAMES_INCLUDE
#include ALL_DEPENDS_INCLUDE_PATH
#include CODES_INCLUDE_PATH
#include EXCEPT_OBJECT_INCLUDE_PATH


namespace core {
	// string conversions, if max_string_buffer is exceeded
	// new is used to allocate a heap buffer to compensate
	// throws exception on error
	const size_t max_string_buffer = MAX_STRING;
	std::wstring to_wide_string(const char* narrow);
	std::wstring to_wide_string(const std::string& narrow);
	std::string to_narrow_string(const wchar_t* wide);
	std::string to_narrow_string(const std::wstring& wide);

	// string conversions with error codes, if max_string_buffer is exceeded
	// new is used to allocate a heap buffer to compensate
	std::wstring to_wide_string(const char* narrow, codes* code_p);
	std::wstring to_wide_string(const std::string& narrow, codes* code_p);
	std::string to_narrow_string(const wchar_t* wide, codes* code_p);
	std::string to_narrow_string(const std::wstring& wide, codes* code_p);

	// get location information
	string get_location(std::source_location sl = std::source_location::current());
	string match_code(codes code);
	void output_code(codes code, const string& location = get_location());
	void output_ce(const core::ce& e);
	void output_se(const std::exception& e);
	void output_hr(HRESULT hr, string location = get_location());

	// translate an hresult code to a string 
	string hr_ts(HRESULT hr);

	uint8_t color_mapper(float t);
}