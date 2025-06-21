/**********************************************************/
//
// File: slowly_api.hpp
//
// Purpose: helper functions for slowly
//
// Project: slowly.core
//
/**********************************************************/

#pragma once
#include CORE_NAMES_INCLUDE
#include CORE_STL_INCLUDE_PATH
#include CORE_CODES_INCLUDE_PATH
#include CORE_KEYWORDS_INCLUDE_PATH

namespace core {
	slowly_package match_in_slowly(keywords key);
	slowly_package match_in_slowly(const std::string& name);
	bool is_keyword(const std::string& keyword, const std::vector<std::string>& container);
	std::vector<std::string> add_tokens(const std::filesystem::path& file,core::codes* code);
	codes translate(std::filesystem::path sl_file);
}