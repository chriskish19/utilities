/**********************************************************/
//
// File: dirt_api.hpp
//
// Purpose: helper functions
//
// Project: dirt.core
//
/**********************************************************/

#pragma once
#include CORE_NAMES_INCLUDE
#include CORE_STL_INCLUDE_PATH
#include CORE_CODES_INCLUDE_PATH
#include CORE_ARGS_INCLUDE_PATH

#include <Windows.h>

namespace core {
	arg_pkg match_arg_enum(args arg);
	arg_pkg match_s_arg(const std::string& arg);
	code_pkg match_code(codes code);
	std::string get_location(std::source_location sl = std::source_location::current());
	void output_em(const code_pkg cp, const std::string location = get_location());
	std::vector<args> validate_args(const std::vector<arg_pkg>& args);
	std::vector<arg_entry> parse_file(const std::filesystem::path& p, core::codes* code_p);
	std::vector<args> arg_pkg_to_args(const std::vector<arg_pkg>& args_pkg_v);
	bool validate_entry(const arg_entry& e);
	std::string get_last_error_w32();
	void output_entry(const arg_entry& e);
	void output_fse(const std::filesystem::filesystem_error& e);
	std::uintmax_t file_numbers(const std::filesystem::path& p);
	std::unordered_set<directory_info> get_all_directories(const std::filesystem::path& p);
	codes copy_directory_only(const std::filesystem::path& dst, const std::filesystem::path& src);
	codes copy_directory_recursive(const std::filesystem::path& dst, const std::filesystem::path& src);
	std::string action_to_string(file_action action);
	void output_entry_data(const file_entry& entry);
	std::string file_type_to_string(std::filesystem::file_type type);
	void output_filesystem_ec(std::error_code ec);
	std::vector<arg_entry> get_specific_entries(const std::vector<arg_entry>& v,args specific_arg);
	std::uintmax_t total_size(const std::filesystem::path& p);
	void progress_dots_in_terminal(int count = 3, int delay_ms = 300, int repeat = 3);
	std::vector<std::queue<file_entry>> split_queue(std::queue<file_entry> buffer_q, std::size_t number_of_qs);
	bool find_directory(const std::filesystem::path& p, const std::filesystem::path& d);

}