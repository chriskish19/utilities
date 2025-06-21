/****************************************************************
	Header: log_window_class.hpp

	Purpose: win32 window that displays log messages.

****************************************************************/


// NOTES: This class is instantiated in the global namespace. So this class header cannot use any global 
// variables or objects within it. (header recurrsive definition error)
/*
* 
* Also these classes cannot use the global instantiated object within either
// class dependencies
#include "main_program_lol.blocks.exe/dependencies/classes/window/starter.hpp"
#include "main_program_lol.blocks.exe/dependencies/classes/utilities/logging_sys.hpp"
*/

#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// class dependencies
#include "main_program_lol.blocks.exe/classes/window/pieces/starter.hpp"
#include "main_program_lol.blocks.exe/classes/utilities/logging_sys.hpp"

namespace window {
	class log_window :
		public starter{
	public:
		log_window();
		~log_window();

		// setup the window and run the message pump
		virtual errors::codes go();
		std::atomic<utilities::logger::logs*>* get_logs_p();
		errors::win32_codes update();
		void set_all_display_windows_closed(bool setting) { m_all_display_windows_closed.store(setting); }
		bool get_all_display_windows_closed() { return m_all_display_windows_closed.load(); }
		errors::codes add_x_log_window();
		HWND get_window_handle() { return m_window_handle; }
	protected:
		std::atomic<bool> m_all_display_windows_closed = false;
		errors::codes remove_x_log_window();
		void create_window() override;
		LRESULT CALLBACK ThisWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		void message_pump() override;
		void draw_log_window();
		void set_font(LONG size);
		void window_size_changed(HWND hwnd);
		utilities::logger m_logs;
		RECT m_client_window_size = {};
		HFONT m_hFont;
		LONG m_font_size = 14;
		LONG m_line_height;

		class scrolling {
		public:
			scrolling(utilities::logger::logs* log_p,LONG line_height);
			void set_scroll_info(HWND hwnd);
			void handle_scroll(HWND hwnd, WPARAM wParam, int wheel_delta);
			void set_line_rect(const RECT& client, const LONG& offset);
			int get_scroll_position() { return m_scroll_pos; }
			RECT create_new_line_rect(const RECT& client, const LONG& offset);
			RECT m_line_rect = {};
		private:
			size_t m_number_of_log_lines;
			int m_scroll_pos = 0;
			int m_wheel_delta_accum = 0;
			LONG m_line_height;
			SCROLLINFO m_si = {};
		};

		scrolling* m_scroll_p = nullptr;
	};
}