/****************************************************************
	Header: go.hpp

	Purpose: keep main.cpp clean.

****************************************************************/


#ifndef GO_HPP
#define GO_HPP

#include "main_program_lol.blocks.exe/pch/pch.hpp"

namespace main {
	// main go function to run whole program
	errors::any_two<errors::codes, errors::win32_codes> go();

	void shutdown();

	void initialize();
}

// GO_HPP
#endif