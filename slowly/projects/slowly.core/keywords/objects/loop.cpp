#include "loop.hpp"
/**********************************************************/
//
// File: loop.cpp
//
// Purpose: loop definition
//
// Project: slowly.core
//
/**********************************************************/

#include CORE_NAMES_INCLUDE
#include CORE_LOOP_INCLUDE_PATH


core::loop::loop(const std::string& code)
	:m_data(code)
{

}

core::codes core::loop::parse()
{
	
}

std::string core::loop::to_cpp()
{
	return std::string();
}

std::string core::loop::extract_code(codes* code)
{
	std::size_t found_open_brace = m_data.find(open_brace_package.m_name);
	std::size_t found_closed_brace = m_data.find(closed_brace_package.m_name);
	if (found_open_brace != std::string::npos and 
		found_closed_brace != std::string::npos) {
		
		found_closed_brace++; // it was an index, now its a length
		std::size_t length = found_closed_brace - found_open_brace;

		try {
			return m_data.substr(found_open_brace, length);
		}
		catch (const std::exception& e) {
			std::cout << "Standard exception caught:\n" << e.what();
		}
		return {};
	}
	else {
		*code = codes::missing_brace;
		return {};
	}
}

core::loop_type core::loop::find_type()
{
	{
		std::size_t found_keyword = m_data.find(array_package.m_name);
		if (found_keyword != std::string::npos) {
			return loop_type::for_each;
		}
	}

	// we need to define var class
}

core::codes core::loop::build(loop_type loop)
{
	switch (loop) {
		case loop_type::basic_for_loop:
		{
			break;
		}

		case loop_type::while_loop:
		{
			break;
		}

		case loop_type::for_each:
		{
			break;
		}

		case loop_type::nested_loop:
		{
			break;
		}

		default:
			break;
	}
}
