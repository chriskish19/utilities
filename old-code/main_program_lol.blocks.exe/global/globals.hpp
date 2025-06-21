/****************************************************************
	Header: globals.hpp

	Purpose: global objects/variables/functions needed for
			 lol.blocks.

****************************************************************/

// NOTE: See class headers for where globals can and cannot be used.

#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// error codes
#include "main_program_lol.blocks.exe/errors/lol.codes.hpp"

// class dependencies
#include "main_program_lol.blocks.exe/classes/window/logging/log_window_class.hpp"
#include "main_program_lol.blocks.exe/classes/window/logging/system_log_window.hpp"



// log window
namespace global {
	
	class global_manager {
	public:
		global_manager() {
			// 2 pointers in this one
			m_log_window_p = new std::atomic<window::log_window*>(new window::system_log_window);
			
			m_all_display_windows_closed = new std::atomic<bool>(false);
		}

		~global_manager() {
			// actual log pointer
			if (m_log_window_p->load() != nullptr) {
				auto actual_log_window_p = m_log_window_p->load();
				delete actual_log_window_p;
				actual_log_window_p = nullptr;
			}

			// std::atomic pointer
			if (m_log_window_p != nullptr) {
				delete m_log_window_p;
				m_log_window_p = nullptr;
			}

			if (m_all_display_windows_closed != nullptr) {
				delete m_all_display_windows_closed;
				m_all_display_windows_closed = nullptr;
			}
		}

		// send a message to the system log window
		void log_to_system_log_window(const string& message) {
			errors::codes code;

			// a message sent to the system log window
			auto log_p = m_log_window_p->load()->get_logs_p();

			code = log_p->load()->log_message(message);
			errors::handle_basic_error_codes(code);

			errors::win32_codes w32code = m_log_window_p->load()->update();
			errors::handle_win32_error_codes(w32code);
		}

		std::atomic<window::log_window*>* get_atomicwrapped_system_log_window_p() { return m_log_window_p; }
		std::atomic<bool>* get_display_windows_closed_p() { return m_all_display_windows_closed; }
	protected:

		std::atomic<window::log_window*>* m_log_window_p = nullptr;
		std::atomic<bool>* m_all_display_windows_closed = nullptr;
	};

	extern global_manager* system_global_manager_p;
}
