/***************************************
*  File: except_object.hpp (exception object)
*
*  Purpose: a throwable error object
*
*  Project: core
* 
* *************************************/

#pragma once
#include NAMES_INCLUDE
#include CODES_INCLUDE_PATH
#include DESCRIPTION_INCLUDE_PATH
#include ALL_DEPENDS_INCLUDE_PATH


namespace core {
	// get location information
	string gl(std::source_location sl = std::source_location::current());

	// returns the current windows error
	string get_w32_error();

	// core error
	class ce : public std::exception{
	public:
		ce() = default;
		ce(codes code, const string& desc, const string& windows_error = get_w32_error(), const string& location = gl() )
			: m_code(code), m_desc(desc),m_w32(windows_error), m_loc(location) { }


		// defaults init
		const codes m_code;
		const string m_desc;
		const string m_loc;
		const string m_w32;

		// helper to convert to wide string
		// cant use api, since api depends on this throwable object for error handling
		static std::wstring to_wide(const std::string& narrow);
	};
}