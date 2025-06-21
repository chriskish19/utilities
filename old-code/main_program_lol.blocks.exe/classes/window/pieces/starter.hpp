/****************************************************************
	Header: starter.hpp

	Purpose: base classes for basic win32 window creation.

****************************************************************/


#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// errors
#include "main_program_lol.blocks.exe/errors/lol.codes.hpp"

namespace window {
	// abstract class
	class foundation {
	public:
		foundation() = default;
	protected:
		virtual LRESULT CALLBACK ThisWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
		virtual void window_settings() = 0;
		virtual void create_window() = 0;
		virtual void message_pump() = 0;

		HWND m_window_handle = nullptr;
		HINSTANCE m_hinst = GetModuleHandle(NULL);
		string m_c_name = L"foundation";
		string m_title = L"plain window";
		WNDCLASS m_wc = {};
	};
	
	
	class starter :public foundation{
	public:
		starter() = default;
		starter(const string& class_name);
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		LRESULT CALLBACK ThisWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

		void window_settings() override;
		void create_window() override;
		void message_pump() override;

		// return value from registering windows class
		inline static std::atomic<ATOM> m_class_atm = 0;
	};
}