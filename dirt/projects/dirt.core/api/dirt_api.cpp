#include "dirt_api.hpp"
/**********************************************************/
//
// File: dirt_api.cpp
//
// Purpose: dirt_api.hpp definitions
//
// Project: dirt.core
//
/**********************************************************/

#include CORE_NAMES_INCLUDE
#include CORE_API_INCLUDE_PATH

core::arg_pkg core::match_arg_enum(args arg)
{
	switch (arg) {
	case args::copy:
		return copy_pkg;
	case args::dirt_list_path:
		return dirt_list_path_pkg;
	case args::mirror:
		return mirror_pkg;
	case args::new_files_only:
		return new_files_only_pkg;
	case args::no_deletes:
		return no_deletes_pkg;
	case args::uninit:
		return uninit_pkg;
	case args::watch:
		return watch_pkg;
	case args::src:
		return src_pkg;
	case args::dst:
		return dst_pkg;
	default:
		return unknown_pkg;
	}
}

core::arg_pkg core::match_s_arg(const std::string& arg)
{
	if (arg == copy_pkg.m_s_arg) {
		return copy_pkg;
	}
	if (arg == dirt_list_path_pkg.m_s_arg) {
		return dirt_list_path_pkg;
	}
	if (arg == mirror_pkg.m_s_arg) {
		return mirror_pkg;
	}
	if (arg == new_files_only_pkg.m_s_arg) {
		return new_files_only_pkg;
	}
	if (arg == no_deletes_pkg.m_s_arg) {
		return no_deletes_pkg;
	}
	if (arg == uninit_pkg.m_s_arg) {
		return uninit_pkg;
	}
	if (arg == watch_pkg.m_s_arg) {
		return watch_pkg;
	}
	if (arg == src_pkg.m_s_arg) {
		return src_pkg;
	}
	if (arg == dst_pkg.m_s_arg) {
		return dst_pkg;
	}
}

core::code_pkg core::match_code(codes code)
{
	switch (code) {
	case codes::success:
		return success_pkg;
	case codes::file_open_fail:
		return file_open_fail_pkg;
	case codes::invalid_file_path:
		return invalid_file_path_pkg;
	case codes::syntax_mistake:
		return syntax_mistake_pkg;
	case codes::uninit:
		return c_uninit_pkg;
	case codes::invalid_cmd_args:
		return invalid_cmd_args_pkg;
	default:
		return c_unknown_pkg;
	}
}

std::string core::get_location(std::source_location sl)
{
	std::string function_name = sl.function_name();
	std::string line = std::to_string(sl.line());
	std::string column = std::to_string(sl.column());
	std::string file_name = sl.file_name();

	std::string temp = std::format("File name: {} \n - column: {} \n - line: {} \n - function name: {} \n", file_name, column, line, function_name);
	return temp;
}

void core::output_em(const code_pkg& cp, const std::string location)
{
	std::cout << cp.m_s_code << '\n' << location << '\n';
}

std::vector<core::args> core::validate_args(const std::vector<arg_pkg>& args)
{
	std::vector<core::args> arg_v;
	for (auto& arg : args) {
		arg_v.push_back(arg.m_arg);
	}

	group_2 g2;
	group_3 g3;
	group_4 g4;
	group_5 g5;

	if (g2.match_group(arg_v) == true or 
		g3.match_group(arg_v) == true or 
		g4.match_group(arg_v) == true or
		g5.match_group(arg_v) == true)
	{
		return arg_v;
	}
	else {
		// empty vector no match group found
		return {};
	}
}
