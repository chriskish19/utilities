/****************************************************************
	Header: memory_sys.hpp

	Purpose: handle memory allocations

****************************************************************/



#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// error codes
#include "main_program_lol.blocks.exe/errors/lol.codes.hpp"


namespace utilities {
	class memory {
	public:
		memory(size_t blob_size);

	protected:
		static const size_t max_blob = 1024u * 1024u * 1024u; // 1GB max
		size_t requested_blob = 0;

		// void* memory_blob_p = new std::byte[max_blob];
	};
}