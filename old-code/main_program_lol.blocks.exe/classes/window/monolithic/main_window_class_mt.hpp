/****************************************************************
	Header: main_window_class_mt.hpp

	Purpose: handles the win32 window system. Creates windows that
			 run on seperate threads. Each with its own message 
			 pump and directx11 device.

****************************************************************/


#ifndef WINDOW_CLASS_MT_HPP
#define WINDOW_CLASS_MT_HPP

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// error handling
#include "main_program_lol.blocks.exe/errors/lol.codes.hpp"

// globals
#include "main_program_lol.blocks.exe/global/globals.hpp"

// class dependencies
#include "main_program_lol.blocks.exe/classes/utilities/thread_manager.hpp"
#include "main_program_lol.blocks.exe/classes/window/logging/log_window_class.hpp"
#include "main_program_lol.blocks.exe/classes/utilities/timer.hpp"
#include "main_program_lol.blocks.exe/classes/window/logging/window_class_log_window.hpp"
#include "main_program_lol.blocks.exe/classes/utilities/templates/function_binder.hpp"

// dx class dependencies
#include "main_program_lol.blocks.exe/dx/dx_device_init.hpp"

// lol api
#include "main_program_lol.blocks.exe/classes/utilities/lol.api.hpp"



namespace window {
	
	class window_class_mt: public utilities::thread_master{
	public:
		// standard constructor
		window_class_mt() = default;

		// objects created with new are deleted here
		~window_class_mt();
		
		// call this function with actual system main thread
		void go(); 

		// call this function with actual system main thread, safely waits...
		// waits until all window_relative windows are closed
		void wait() noexcept; 
	protected:
		// class global object
		// mechansims for safe multithreading in window_class_mt
		// we create windows on new threads so we need to know whats happended
		class latch {
		public:
			// when its safe to exit
			std::condition_variable m_safe_exit;
			std::atomic<bool> m_safe_exit_gate_latch = false;

			// when a new window needs to be created
			std::condition_variable m_window_create_signaler;
			std::atomic<bool> m_new_window_gate_latch = false;
		};
	public:
		// main latches object
		std::atomic<latch*>* m_wcmt_latches = new std::atomic<latch*>(new latch);
	protected:
		class window_relative {
		public:
			// default constructor: currently not used
			window_relative() = default;

			// constructor that is currently being used to build win32 windows
			window_relative(const string& title,std::atomic<latch*>* latches_p);

			// clean up new objects
			~window_relative();

			// title
			void change_title(const string& new_title) noexcept;

			// HWND current
			HWND get_window_handle() noexcept { return m_window_handle; }

			// main window proc rerouter
			static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			
			// call this function once to build the win32 menu
			// on a window_relative
			errors::win32_codes build_relative_window_menu_bar();

			// runs the display window rendering
			void run_window_logic(dx::devices_11* dx11_device_p, log_window* log_p);

			// while loop boolean variable in run_window_logic()
			// also run_window_logic_draw_primitives()
			std::atomic<bool> m_public_exit_run_window_logic = false;

			// returns width of window_relative
			// calls GetClientRect() (not the fastest)
			UINT get_window_width();

			// returns height of window_relative
			// calls GetClientRect() (not the fastest)
			UINT get_window_height();

			void close_window();
		private:
			// return value from registering the class
			inline static std::atomic<ATOM> m_class_atm = 0;

			// this is for the log window menu button
			// shows/hides the display log window
			bool m_show_log_window = false;
			errors::codes view_log_window(bool show);

			// main window procedure
			LRESULT CALLBACK PrivateWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

			// calls RegisterClass(&m_wc);
			void register_class();

			// HWND
			HWND m_window_handle = nullptr;

			// window class description
			WNDCLASS m_wc = {};

			// unique window class name
			string m_c_name = READ_ONLY_STRING("main_window_class_mt_window");

			// title of the window (window bar name)
			string m_title;

			// main exe handle
			HINSTANCE m_hinst = GetModuleHandle(NULL);

			// safe multithreading mechanisms
			std::atomic<latch*>* m_latches;

			// set inside run_window_logic()
			// these pointers allow drawing and changes to the window
			// wrapped in std::atomic for safe accessing
			/*
			
				std::atomic<log_window*> m_log_window_p = nullptr;
				std::atomic<dx::devices_11*> m_dx11_device_p = nullptr;
				std::atomic<dx::draw*> m_dx_draw_p = nullptr;

			*/

			// log window that receives messages from display window (window_relative)
			std::atomic<log_window*> m_log_window_p = nullptr;

			// main direct x device class pointer
			std::atomic<dx::devices_11*> m_dx11_device_p = nullptr;

			// window menu button ids
			enum class window_menu_ids {
				File,
				Help,
				Create,
				View,
				view_log_window
			};

			// swap chain pointer for drawing purposes
			IDXGISwapChain* m_swp_p = nullptr;
		};

		// main object for handling window_relative objects
		// when the thread that launches windows_message_handler()
		// it gets signalled by thread master main thread to close
		class window_manager {
		public:
			window_manager(std::atomic<latch*>* latches_p) noexcept
				:m_latches(latches_p) {
			}

			window_manager() = default;

			// basic running of logic
			void windows_message_handler();

			// count of how many display windows are currently open
			std::atomic<unsigned int> m_open_window_count = 0;

			// signals and gates for when all display windows close
			std::atomic<bool> m_all_windows_closed_gate_latch = false;
			std::mutex m_all_windows_close_same_time;
			std::condition_variable m_public_all_windows_closed_signaler;

			// gives window manager access to other signals
			std::atomic<latch*>* m_latches = nullptr;

#if TESTING
			// keep track of window pointers
			std::vector<window_relative*> m_windows_vec;
#endif
		};

		// this class is used to package the thread running functions
		class run_windows_class_mt : public utilities::thread_master {
		public:
			run_windows_class_mt(std::atomic<latch*>* latches_p) noexcept
			:m_wm(new window_manager(latches_p)){}

			run_windows_class_mt() = default;
			~run_windows_class_mt();

			// threads launched here
			void threads_go();

			// when all display windows are closed the thread can exit this function
			void all_windows_closed_gate() noexcept;

			// creates new windows and launches them on a new thread
			void new_window_gate() noexcept;

			// creates a new window on a new thread
			void launch_new_window();
		
			// while loop condition in new_window_gate
			std::atomic<bool> m_exit_new_window_gate = false;
			
			// allocated with new in constructor
			window_manager* m_wm;
		};

		// main object that runs window_class_mt
		// handles the threads that run the functions needed for window_class_mt
		run_windows_class_mt* m_thread_runner = new run_windows_class_mt(m_wcmt_latches);

	};
}

// WINDOW_CLASS_MT_HPP
#endif