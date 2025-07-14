/**********************************************************/
//
// File: var.hpp
//
// Purpose: define a variable in slowly
//
// Project: slowly.core
//
/**********************************************************/

#pragma once
#include CORE_NAMES_INCLUDE
#include CORE_STL_INCLUDE_PATH

namespace core {
	class var {
	public:
		var(const std::string& code);


	protected:
		std::string m_code;

	};
}