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


core::loop::loop(const std::string& data)
	:m_data(data)
{

}

core::codes core::loop::parse()
{
	return codes();
}

std::string core::loop::to_cpp()
{
	return std::string();
}
