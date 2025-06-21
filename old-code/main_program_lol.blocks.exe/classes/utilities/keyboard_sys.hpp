/****************************************************************
	Header: keyboard_sys.hpp

	Purpose: handles keystrokes between the window and graphics
			 logic.

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

namespace utilities {
	class keyboard {
	public:
		keyboard() = default;

		enum class em_key_state {
			released,
			pressed
		};

		class key_event {
		public:
			char key;
			em_key_state state;
		};

		errors::codes add_event(key_event kp);

		// run this in the graphics logic??
		errors::codes system_message_handler();
	private:
		std::queue<key_event> m_key_queue;
	};
}