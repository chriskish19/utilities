/****************************************************************
	Header: system_log_window.hpp

	Purpose: creates a log window with its own message pump.

****************************************************************/




#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// error handling
#include "main_program_lol.blocks.exe/errors/lol.codes.hpp"

// class dependencies
#include "main_program_lol.blocks.exe/classes/window/logging/log_window_class.hpp"

namespace window {
	class system_log_window : public log_window {
	public:
		system_log_window() = default;

		// to use this run it on a new thread
		// setup the window and run message pump
		errors::codes go() override;

		// sends a message to close log window
		errors::codes shutdown();

		// causes the thread to wait until system log window is fully 
		// initialized before moving ahead to next lines of code
		errors::codes wait_until_init();
	protected:
		void window_settings() override;
		void create_window() override;
		WNDCLASS m_slw_wc = {};
		string m_slw_class_name = READ_ONLY_STRING("system log window");
		string m_slw_title = READ_ONLY_STRING("SYSTEM LOG WINDOW");

		// waiting gates and latches
		std::condition_variable m_fully_init;
		std::atomic<bool> m_fully_init_gate_latch = false;
	};
}