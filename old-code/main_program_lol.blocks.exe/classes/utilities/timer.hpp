/****************************************************************
	Header: timer.hpp

	Purpose: simple time tracker

****************************************************************/



#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// error codes
#include "main_program_lol.blocks.exe/errors/lol.codes.hpp"

// globals
#include "main_program_lol.blocks.exe/global/globals.hpp"

namespace utilities{
	class timer {
	public:
		timer();

		float peek();
	private:

		UINT m_delta_ms = 0;
		UINT m_delta_s = 0;

		std::chrono::steady_clock::time_point m_last;
	};
}