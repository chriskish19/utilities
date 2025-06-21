/****************************************************************
	Header: file_manager.hpp

	Purpose: xxxx

****************************************************************/

#pragma once

// all macros
#include "main_program_lol.blocks.exe\macros\include\lol.macros.include.hpp"

// all library
#include "main_program_lol.blocks.exe\pch\includes\external.hpp"

// errors
#include "main_program_lol.blocks.exe\errors\lol.codes.hpp"

// helpers
#include "main_program_lol.blocks.exe\classes\utilities\lol.api.hpp"

namespace utilities {
	class file_manager {
	public:
		file_manager(const std::filesystem::path& file_path);
		~file_manager() {
			m_file_stream.close();
		}

		errors::codes open(std::ios_base::openmode mode);

		string file_data_to_string();
	protected:
		fs::path m_file_path;
		std::fstream m_file_stream;
	};
}