/****************************************************************
	Header: file_logger.hpp

	Purpose: xxxx

****************************************************************/

#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// classes
#include "main_program_lol.blocks.exe/classes/utilities/logging_sys.hpp"

// errors
#include "main_program_lol.blocks.exe/errors/lol.codes.hpp"

namespace utilities {
	class file_logs : public logger {
	public:
		file_logs();
		~file_logs() = default;

		file_logs(const std::filesystem::path& log_file_path);

		errors::codes flush();
		errors::codes write_log_message(const string& message);

	protected:
		std::filesystem::path m_log_file_path;
	
	private:
		errors::codes create_log_file();

		const string m_file_name = READ_ONLY_STRING("log_file.txt");
	};
}