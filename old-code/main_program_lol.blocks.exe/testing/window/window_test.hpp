/****************************************************************
	Header: window_test.hpp

	Purpose: test window creation system

****************************************************************/

#pragma once

#include "main_program_lol.blocks.exe\classes\window\monolithic\main_window_class_mt.hpp"

namespace testing {
	class window_t : public window::window_class_mt {
	public:
		window_t() = default;

		void create_windows(std::size_t number) {
			for (std::size_t i = 0; i < number; ++i) {
				m_thread_runner->launch_new_window();
			}
		}

		void close_windows() {
			for (auto window : m_thread_runner->m_wm->m_windows_vec) {
				window->close_window();
			}
		}
	protected:

	};
}