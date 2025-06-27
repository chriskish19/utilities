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
	case args::recursive:
		return recursive_pkg;
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
	if (arg == recursive_pkg.m_s_arg) {
		return recursive_pkg;
	}
	return unknown_pkg;
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
	case codes::invalid_io_handle:
		return invalid_io_handle_pkg;
	case codes::no_valid_entries:
		return no_valid_entries_pkg;
	case codes::read_dir_changes_fail:
		return read_dir_changes_fail_pkg;
	case codes::std_filesystem_exception_caught:
		return std_filesystem_exception_caught_pkg;
	case codes::invalid_directory_path:
		return invalid_directory_path_pkg;
	case codes::unknown_exception_caught:
		return unknown_exception_caught_pkg;
	case codes::exception_thrown_and_handled:
		return exception_thrown_and_handled_pkg;
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

std::vector<core::arg_entry> core::parse_file(const std::filesystem::path& p, core::codes* code_p)
{
	std::ifstream file;
	file.open(p);

	if (file.is_open() == false) {
		*code_p = codes::file_open_fail;
		return {};
	}

	arg_entry entry;
	std::vector<arg_entry> entry_v;
	std::string line;
	std::size_t entry_number = 0;
	std::string last_word;

	while (std::getline(file, line)) {
		// ignore blank lines
		if (line.empty() == true) {
			continue;
		}

		// ignore comments
		std::size_t comment_slash_pos = line.find_first_of("//");
		std::size_t comment_start_pos = line.find_first_not_of("//");
		std::size_t comment_end_pos = line.find_first_of("\n");
		if (comment_start_pos != std::string::npos 
			and comment_slash_pos != std::string::npos
			and comment_end_pos != std::string::npos) {
			// we have a comment so skip the line
			continue;
		}

		std::string word;
		std::istringstream iss(line);
		while (iss >> word) {
			
			auto found = core::gbl_args_mp.find(word);
			if (found != gbl_args_mp.end()) {
				entry.args_v.push_back(found->second);
			}
			else if(word == "args"){
				last_word = word;
				continue;
			}
			else if (word == "{") {
				// the next word should be src
				last_word = word;
				continue;
			}
			


			if (last_word == "args" and word == ":") {
				// the next words should be arguments
				while (iss >> word) {
					auto found = core::gbl_args_mp.find(word);
					if (found != gbl_args_mp.end()) {
						entry.args_v.push_back(found->second);
					}
				}
				
			}

			if (last_word == "{" and word == "src") {
				// the next should be the src path
				iss >> word;
				entry.src_p = word;
			}

			if (word == "dst") {
				// the next should be the dst path
				iss >> word;
				entry.dst_p = word;
			}
		}

		if (line.find_first_of("}") != std::string::npos) {
			entry_number++;
			entry.entry_number = entry_number;
			
			// end of entry
			entry_v.push_back(entry);
			entry = arg_entry(); // reset
		}
	}

	*code_p = codes::success;
	return entry_v;
}

std::vector<core::args> core::arg_pkg_to_args(const std::vector<arg_pkg>& args_pkg_v)
{
	std::vector<core::args> args_v;
	for (auto& arg : args_pkg_v) {
		args_v.push_back(arg.m_arg);
	}
	return args_v;
}

bool core::validate_entry(const arg_entry& e)
{
	
	group_2 g2;
	group_3 g3;
	group_4 g4;
	group_5 g5;

	if (g2.match_group(e.args_v) == true or
		g3.match_group(e.args_v) == true or
		g4.match_group(e.args_v) == true or
		g5.match_group(e.args_v) == true)
	{}
	else {
		return false;
	}

	return	std::filesystem::is_directory(e.src_p) and
			std::filesystem::is_directory(e.dst_p);
}

std::string core::get_last_error_w32()
{
	DWORD errorMessageID = GetLastError();
	if (errorMessageID == 0) {
		return {}; // No error message has been recorded 
	}

	LPSTR messageBuffer = nullptr;

	// Format the error message 
	size_t size = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorMessageID,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&messageBuffer,
		0,
		NULL
	);

	// Copy the error message into a wide string
	std::string message(messageBuffer, size);

	// Free the buffer allocated by FormatMessage 
	LocalFree(messageBuffer);

	return message;
}

void core::output_entry(const arg_entry& e)
{
	std::vector<std::string> s_v;
	for (auto arg : e.args_v) {
		auto pkg = match_arg_enum(arg);
		s_v.push_back(pkg.m_s_arg);
	}

	// outputing to the console:
	std::cout << "Entry number: " << e.entry_number << '\n';
	for (const auto& s : s_v) {
		std::cout << "arg: " << s << '\n';
	}
	std::cout << "Destination Path: " << e.dst_p << '\n';
	std::cout << "Source Path: " << e.src_p << '\n';
	
}

void core::output_fse(const std::filesystem::filesystem_error& e)
{
	std::cout << "Message: " << e.what() << '\n' << "Path 1: " 
		<< e.path1() << '\n' << "Path 2: " << e.path2() << '\n';
}

std::uintmax_t core::file_numbers(const std::filesystem::path& p)
{
	try {
		std::uintmax_t count = 0;
		if (std::filesystem::exists(p) && std::filesystem::is_directory(p)) {
			for (const auto& entry : std::filesystem::directory_iterator(p)) {
				if (entry.is_regular_file()) {
					++count;
				}
			}
		}
		return count;
	}
	catch (const std::filesystem::filesystem_error& e) {
		output_em(std_filesystem_exception_caught_pkg);
		output_fse(e);
	}
	catch (...) {
		std::cout << "unknown exception caught...\n";
	}
	return 0;
}

std::unordered_set<core::directory_info> core::get_all_directories(const std::filesystem::path& p)
{
	try {
		if (std::filesystem::exists(p) == false or std::filesystem::is_directory(p) == false) {
			return {};
		}

		std::unordered_set<core::directory_info> di_set;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(p)) {
			directory_info di;
			if (entry.is_directory() == true) {
				di.p = entry.path();
				di.number_of_files = file_numbers(entry.path());
				di_set.emplace(di);
			}
		}

		return di_set;
	}
	catch (const std::filesystem::filesystem_error& e) {
		output_em(std_filesystem_exception_caught_pkg);
		output_fse(e);
	}
	catch (...) {
		std::cout << "unknown exception caught...\n";
	}
	return {};
}

void core::background_task(const file_entry& entry)
{
	switch (entry.completed_action) {
	case directory_completed_action::recursive_copy:
	{
		try {
			auto set = entry.p_di_set;
			for (const auto& i : std::filesystem::recursive_directory_iterator(entry.dst_p)) {
				directory_info di;
				if (i.is_directory() == true) {
					di.number_of_files = file_numbers(i.path());
					di.p = i.path();
					di.action = entry.completed_action;
					set->emplace(di);
				}
			}
		}
		catch (const std::filesystem::filesystem_error& e) {
			output_em(std_filesystem_exception_caught_pkg);
			output_fse(e);
		}
		catch (...) {
			std::cout << "unknown exception caught...\n";
		}
		break;
	}

	case directory_completed_action::delete_all:
	{
		try {
			auto set = entry.p_di_set;
			for (const auto& i : std::filesystem::recursive_directory_iterator(entry.dst_p)) {
				directory_info di;
				if (i.is_directory() == true) {
					di.number_of_files = file_numbers(i.path());
					di.p = i.path();
					di.action = entry.completed_action;
					set->erase(di);
				}
			}
		}
		catch (const std::filesystem::filesystem_error& e) {
			output_em(std_filesystem_exception_caught_pkg);
			output_fse(e);
		}
		catch (...) {
			std::cout << "unknown exception caught...\n";
		}
		break;
	}

	case directory_completed_action::previous_name:
	{

		break;
	}

	case directory_completed_action::new_name:
	{

		break;
	}

	default:
		// do
		break;
	}
}

core::codes core::copy_directory_only(const std::filesystem::path& dst, const std::filesystem::path& src)
{
	try {
		if (std::filesystem::is_directory(dst) == false or std::filesystem::is_directory(src) == false) {
			return codes::invalid_directory_path;
		}

		std::filesystem::copy(src, dst, std::filesystem::copy_options::update_existing);

		return codes::success;
	}
	catch (const std::filesystem::filesystem_error& e) {
		output_em(std_filesystem_exception_caught_pkg);
		output_fse(e);
	}
	catch (...) {
		output_em(unknown_exception_caught_pkg);
		std::cout << "unknown exception caught...\n";
	}
	return codes::exception_thrown_and_handled;
}

core::codes core::copy_directory_recursive(const std::filesystem::path& dst, const std::filesystem::path& src)
{
	try {
		if (std::filesystem::is_directory(dst) == false or std::filesystem::is_directory(src) == false) {
			return codes::invalid_directory_path;
		}

		std::filesystem::copy(src, dst, 
			std::filesystem::copy_options::update_existing |
			std::filesystem::copy_options::recursive);

		return codes::success;
	}
	catch (const std::filesystem::filesystem_error& e) {
		output_em(std_filesystem_exception_caught_pkg);
		output_fse(e);
	}
	catch (...) {
		output_em(unknown_exception_caught_pkg);
		std::cout << "unknown exception caught...\n";
	}
	return codes::exception_thrown_and_handled;
}
