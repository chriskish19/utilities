#include "go.hpp"

errors::any_two<errors::codes,errors::win32_codes> main::go() {
	// global logging window messages throughout whole program
	auto atomic_log_window_p = global::system_global_manager_p->get_atomicwrapped_system_log_window_p();
	auto log_window_p = atomic_log_window_p->load();
	std::thread lw_thread(&window::log_window::go,log_window_p );

	// main window class
	window::window_class_mt* local_window_mt_system_p = new window::window_class_mt;
	local_window_mt_system_p->go();


	// waits until all display windows are closed
	local_window_mt_system_p->wait();
	atomic_log_window_p->load()->set_all_display_windows_closed(true);
	auto all_display_windows_closed = global::system_global_manager_p->get_display_windows_closed_p();
	all_display_windows_closed->store(atomic_log_window_p->load()->get_all_display_windows_closed());
	atomic_log_window_p->load()->add_x_log_window();
	if (local_window_mt_system_p != nullptr) {
		delete local_window_mt_system_p;
		local_window_mt_system_p = nullptr;
	}

	// a message sent to the system log window
	global::system_global_manager_p->
		log_to_system_log_window(READ_ONLY_STRING("All display windows closed. Close this window to exit program."));

	// system log window can stay open to show possible errors
	if (lw_thread.joinable()) {
		lw_thread.join();
	}

	// the return values of the functions in here need some error checking
	return errors::any_two<errors::codes, errors::win32_codes>(errors::codes::success, errors::win32_codes::success);
}

void main::shutdown()
{
	// delete global pointer
	if (global::system_global_manager_p != nullptr) {
		delete global::system_global_manager_p;
	}

	// deletes error handling
	code_error_objs::clean_up();
	win32_code_objs::clean_up();

}

void main::initialize()
{
	
	/*
	
	char* setlocale( int category, const char* locale );

	category 	- 	locale category identifier, one of the LC_xxx macros. May be ​0​.
	locale 	- 	system-specific locale identifier. Can be "" for the user-preferred locale or "C" for the minimal locale.

	The setlocale function installs the specified system locale or its portion as the new C locale. 
	The modifications remain in effect and influences the execution of all locale-sensitive C library 
	functions until the next call to setlocale. If locale is a null pointer, setlocale queries the 
	current C locale without modifying it.

	Return value:
	Pointer to a narrow null-terminated string identifying the C locale after applying the changes, 
	if any, or null pointer on failure.

	A copy of the returned string along with the category used in this call to std::setlocale 
	may be used later in the program to restore the locale back to the state at the end of this call.

	*/
	
	char* locale = std::setlocale(LC_ALL, "en_US.utf8");  // Ensure locale supports conversion

	if (locale == nullptr) {
#if ENABLE_ALL_EXCEPTIONS
		{
			code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
			throw errors::pointer_is_nullptr(
				error, READ_ONLY_STRING("char* locale = std::setlocale(LC_ALL, \"en_US.utf8\");"));
		}
#endif

#if ENABLE_ERROR_OUTPUT_WINDOW
		{
			code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
			errors::pointer_is_nullptr p_null_error(
				error, READ_ONLY_STRING("char* locale = std::setlocale(LC_ALL, \"en_US.utf8\");"));
			errors::show_error_message_window(p_null_error.full_error_message());
		}
#endif
	}
}
