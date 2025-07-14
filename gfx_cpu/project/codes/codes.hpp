/***********************************************
* File: codes.hpp
*
* Purpose: return codes for core
*
* Project: core
*
************************************************/

#pragma once


namespace core {
	enum class codes {
		success = 0,
		uninitialized,
		register_class_fail,
		hwnd_error,
		to_wide_string_failed,
		to_narrow_string_failed,
		string_length_too_long,
		match_code_default,
		get_client_rect_fail,
		unknown_exception_caught,
		std_exception_caught,
		pointer_is_nullptr,
	};
}