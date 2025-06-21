/****************************************************************
	Header: basic_window.hpp

	Purpose: a window for testing direct x

****************************************************************/

#pragma once

// includes all
#include "main_program_lol.blocks.exe/pch/pch.hpp"

namespace testing {
	class basic_window : public window::starter {
	public:
		basic_window() {
			this->window_settings();
			this->create_window();
		}

		UINT width();
		UINT height();

		HWND window_handle() { return m_window_handle; }
	protected:
		LRESULT CALLBACK ThisWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	};
}