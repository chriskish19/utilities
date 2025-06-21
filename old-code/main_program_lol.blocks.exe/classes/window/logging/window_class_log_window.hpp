/****************************************************************
	Header: window_class_log_window.hpp

	Purpose: creates a log window without a message pump.
			 Used in window_class_mt.hpp for logging messages
			 from a display window.

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

// globals
#include "main_program_lol.blocks.exe/global/globals.hpp"

namespace window {

	class window_class_log_window : public log_window {
	public:
		window_class_log_window(const string& title);
		~window_class_log_window();

		// this is designed to run alongside a window
		// use the same thread that you created a regular window with 
		// and create this window then the same message pump loop will work 
		// for both window_class_log_window and a regular window.
		// setup the window with no message pump
		errors::codes go() override;
	private:
		void window_settings() override;
		void create_window() override;
		string m_window_title;
		WNDCLASS m_wclw_wc = {};
		string m_class_name = READ_ONLY_STRING("window class log window");
		inline static std::atomic<ATOM> m_class_atm = 0;
	};
}