/**********************************************************/
//
// File: cmdline.hpp
//
// Purpose: parse command line arguments
//
// Project: dirt.core
//
/**********************************************************/

#pragma once
#include CORE_NAMES_INCLUDE
#include CORE_STL_INCLUDE_PATH
#include CORE_ARGS_INCLUDE_PATH
#include CORE_API_INCLUDE_PATH


namespace core {
	class cmdline {
	public:
		cmdline(int argc, char* argv[]);

		codes parse();

		bool is_using_list_file() { return m_dirt_lists_used; }

		codes validate();

		std::vector<core::arg_entry> m_entrys_v;
		core::arg_entry m_cmdline_entry;
	protected:
		std::vector<std::string> m_args_v;
		bool m_next_is_dirt_path = false;
		std::filesystem::path m_dirt_list_txt_path;
		bool m_dirt_lists_used = false;
	};
}