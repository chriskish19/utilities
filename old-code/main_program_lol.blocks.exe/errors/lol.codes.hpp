/****************************************************************
	Header: lol.codes.hpp

	Purpose: error handling for the whole program

****************************************************************/

// NOTES: This header is designed to be used anywhere and everywhere in lol.blocks
// it doesnt depend on any classes...


#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"


namespace w32errors_cstr {
	// win32_codes string explainations
	inline const character* success = READ_ONLY_STRING("win32_codes: success, successful execution");
	inline const character* menu_error = READ_ONLY_STRING("win32_codes: menu_error, an issue with when creating a menu item using CreateMenu()");
	inline const character* HWND_error = READ_ONLY_STRING("win32_codes: HWND_error, the window handle is nullptr");
	inline const character* register_class_fail = READ_ONLY_STRING("win32_codes: register_class_fail, issue when calling RegisterClass()");
	inline const character* get_client_rect_fail = READ_ONLY_STRING("win32_codes: get_client_rect_fail, issue when trying to obtain the client window size using GetClientRect()");
	inline const character* invalidate_rect_fail = READ_ONLY_STRING("win32_codes: invalidate_rect_fail, issue when calling the function InvalidateRect()");
	inline const character* font_error = READ_ONLY_STRING("win32_codes: font_error, a problem when creating a new font. The function CreateFont() failed");
	inline const character* null = READ_ONLY_STRING("win32_codes: null, n/a");
}

namespace errors_cstr {
	// regular codes string explainations
	inline const character* success = READ_ONLY_STRING("codes: success, successful execution");
	inline const character* pointer_is_nullptr = READ_ONLY_STRING("codes: pointer_is_nullptr, pointer has no memory to point to");
	inline const character* wide_string_copy_fail_wcs_cpy = READ_ONLY_STRING("codes: wide_string_copy_fail_wcs_cpy, wide string copy function failed to copy the string");
	inline const character* strings_not_equal = READ_ONLY_STRING("codes: strings_not_equal, the two strings you are comparing are different in size or characters");
	inline const character* empty_string = READ_ONLY_STRING("codes: empty_string, the string is empty. no characters.");
	inline const character* string_length_too_long = READ_ONLY_STRING("codes: string_length_too_long, the strings length is too long for this function to procced");
	inline const character* index_out_of_range = READ_ONLY_STRING("codes: index_out_of_range, you are trying to access an array/vector using an index that is non existant in the array/vector");
	inline const character* division_by_zero = READ_ONLY_STRING("codes: division_by_zero, trying to divide by zero is undefined");
	inline const character* unknown_keyboard_key_in_system_message_handler = READ_ONLY_STRING("codes: unknown_keyboard_key_in_system_message_handler, where key presses are handled in the program a key has been pressed that is not known");
	inline const character* unknown_mouse_button_in_system_message_handler = READ_ONLY_STRING("codes: unknown_mouse_button_in_system_message_handler, where mouse button presses are handled in the program a mouse button has been pressed which is not known");
	inline const character* dx_error = READ_ONLY_STRING("codes: dx_error, a direct x error has occurred");
	inline const character* exception_thrown_and_handled = READ_ONLY_STRING("codes: exception_thrown_and_handled, an exception has occurred and has been caught and handled");
	inline const character* to_wide_string_failed = READ_ONLY_STRING("codes: to_wide_string_failed, converting a narrow string to wide string has failed");
	inline const character* to_narrow_string_failed = READ_ONLY_STRING("codes: to_narrow_string_failed, converting a wide string to narrow has failed");
	inline const character* memory_leak = READ_ONLY_STRING("codes: memory_leak, allocated memory but didnt call delete on it!");
	inline const character* filesystem_error = READ_ONLY_STRING("codes: filesystem_error, an error doing file operations has occurred. Check file system error codes.");
	inline const character* test_success = ROS("codes: test_success, no errors test completed successfully...");
	inline const character* test_failed = ROS("codes: test_failed, error occurred during testing");
}


/*  helpful functions for error objects */ 
/****************************************/
namespace errors {
	// these functions are meant to be used only within lol.codes.hpp

	// returns the location of the function call site, used in exceptions classes constructor
	string get_location(std::source_location sl = std::source_location::current());

	// if there is no error an empty string is returned
	string get_last_error_win32();

	// if these functions fail they return an empty string.
	// if max_string_buffer is exceeded then new is used to allocate memory
	const std::size_t max_string_buffer = MAX_STRING;
	std::wstring to_wide_string(const std::string& narrow);
	std::string to_narrow_string(const std::wstring& wide);

	// a message box window that will display errors, for when system log window cant be used
	// or for pessky debug errors
	void show_error_message_window(const string& message, const string& title=READ_ONLY_STRING("ERROR WINDOW"));
}


/*  codes and dependant templates		*/
/****************************************/
namespace errors {
	template<typename t1, typename t2>
	class any_two {
	public:
		any_two() = default;
		any_two(const t1& one,const t2& two) noexcept
			:m_one(one), m_two(two) {
		}

		t1 m_one;
		t2 m_two;
	};


	template<typename t>
	class basic_error : public std::exception {
	public:
		basic_error() = default;
		basic_error(const any_two<t,string>& code) noexcept
			:m_error_code(code){}

		basic_error(const t code,const string& message) noexcept {
			m_error_code.m_one = code;
			m_error_code.m_two = message;
		}

		virtual string full_error_message() noexcept{
			return std::format(READ_ONLY_STRING("error message: {}"), m_error_code.m_two);
		}
	protected:
		any_two<t,string> m_error_code;
	};

	
	// basic random error code mesages
	// most are class objects with the same names below
	// they are also exceptions
	enum class codes {
		success = 0,
		pointer_is_nullptr,
		wide_string_copy_fail_wcs_cpy,
		strings_not_equal,
		empty_string,
		string_length_too_long,
		index_out_of_range,
		division_by_zero,
		unknown_keyboard_key_in_system_message_handler,
		unknown_mouse_button_in_system_message_handler,
		dx_error,
		exception_thrown_and_handled,
		to_wide_string_failed,
		to_narrow_string_failed,
		memory_leak,
		filesystem_error,
		test_failed,
		test_success
	};

	enum class win32_codes {
		success = 0,
		menu_error,
		HWND_error,
		register_class_fail,
		get_client_rect_fail,
		invalidate_rect_fail,
		font_error,
		null
	};
}

/* regular code error objects */
/******************************/
namespace code_error_objs {
	using namespace errors;

	extern any_two<errors::codes, string>* success;
	extern any_two<errors::codes, string>* pointer_is_nullptr;
	extern any_two<errors::codes, string>* wide_string_copy_fail_wcs_cpy;
	extern any_two<errors::codes, string>* strings_not_equal;
	extern any_two<errors::codes, string>* empty_string;
	extern any_two<errors::codes, string>* string_length_too_long;
	extern any_two<errors::codes, string>* index_out_of_range;
	extern any_two<errors::codes, string>* division_by_zero;
	extern any_two<errors::codes, string>* unknown_keyboard_key_in_system_message_handler;
	extern any_two<errors::codes, string>* unknown_mouse_button_in_system_message_handler;
	extern any_two<errors::codes, string>* dx_error;
	extern any_two<errors::codes, string>* exception_thrown_and_handled;
	extern any_two<errors::codes, string>* to_wide_string_failed;
	extern any_two<errors::codes, string>* to_narrow_string_failed;
	extern any_two<errors::codes, string>* memory_leak;
	extern any_two<errors::codes, string>* filesystem_error;
	extern any_two<errors::codes, string>* test_success;
	extern any_two<errors::codes, string>* test_failed;


	void clean_up();

	class code_obj {
	public:
		code_obj(any_two<codes,string>* code) noexcept
			:m_code(*code){ }

		any_two<codes, string> output() {
			return m_code;
		}
	protected:
		any_two<codes, string> m_code;
	};
}

namespace win32_code_objs {
	using namespace errors;

	extern any_two<win32_codes, string>* success;
	extern any_two<win32_codes, string>* menu_error;
	extern any_two<win32_codes, string>* HWND_error;
	extern any_two<win32_codes, string>* register_class_fail;
	extern any_two<win32_codes, string>* get_client_rect_fail;
	extern any_two<win32_codes, string>* invalidate_rect_fail;
	extern any_two<win32_codes, string>* font_error;

	void clean_up();

	class code_obj {
	public:
		code_obj(any_two<win32_codes, string>* code) noexcept
			:m_code(*code) {
		}

		any_two<win32_codes, string> output() noexcept{
			return m_code;
		}
	protected:
		any_two<win32_codes, string> m_code;
	};

	
}




/*  win32 error objects	*/
/************************/
namespace errors {

	class win32_error : public basic_error<win32_codes> {
	public:
		win32_error() = default;
		win32_error(win32_code_objs::code_obj& code, const string& location = errors::get_location()) noexcept
			:basic_error<win32_codes>(code.output()), m_location(location) {}
		string full_error_message() noexcept override {
			return std::format(READ_ONLY_STRING("error message: {}\n, location: {}\n,w32apierr: {}"), m_error_code.m_two, m_location, w32_err_str);
		}
	protected:
		string m_location;
		string w32_err_str = get_last_error_win32();
	};


	class font_error : public win32_error {
	public:
		font_error(win32_code_objs::code_obj& code, const string& location = errors::get_location()) noexcept
			:win32_error(code, location) { }
	};

	class get_client_rect_failed : public win32_error {
	public:
		get_client_rect_failed(win32_code_objs::code_obj& code, const string& location = errors::get_location()) noexcept
			:win32_error(code,location){ }
	};

	class invalidate_rect_failed : public win32_error {
	public:
		invalidate_rect_failed(win32_code_objs::code_obj& code, const string& location = errors::get_location()) noexcept
			:win32_error(code, location) { }
	};

	class win32_menu_error : public win32_error {
	public:
		win32_menu_error(win32_code_objs::code_obj& code, const string& location = errors::get_location()) noexcept
			:win32_error(code, location) { }
	};

	class win32_HWND_error : public win32_error {
	public:
		win32_HWND_error(win32_code_objs::code_obj& code, const string& location = errors::get_location()) noexcept
			:win32_error(code, location) { }
	};

	class win32_register_class_fail : public win32_error {
	public:
		win32_register_class_fail(win32_code_objs::code_obj& code, const string& location = errors::get_location()) noexcept
			:win32_error(code, location) { }
	};
}

/* basic error objects */
/***********************/
namespace errors{

	class pointer_is_nullptr : public basic_error<codes> {
	public:
		pointer_is_nullptr(code_error_objs::code_obj& code,const string& p_name,const string& location=errors::get_location()) noexcept
			:basic_error<codes>(code.output()),m_pointer_name(p_name),m_location(location){ }

		string get_pointer_variable_name() noexcept{ return m_pointer_name; }
		string full_error_message() noexcept override {
			return std::format(READ_ONLY_STRING("error message: {}\n, location: {}\n, pointer name: {}"), m_error_code.m_two, m_location, m_pointer_name);
		}
	protected:
		string m_location;
		string m_pointer_name;
	};


	class strings_not_equal : public basic_error<codes> {
	public:
		strings_not_equal(code_error_objs::code_obj& code,const string& one,const string& two,const string& location=get_location()) noexcept
			:basic_error<codes>(code.output()),m_one(one),m_two(two),m_location(location){ }

		string full_error_message() noexcept override {
			return std::format(READ_ONLY_STRING("error message: {}\n, location: {}\n, strings: {}\n,{}"), m_error_code.m_two, m_location, m_one, m_two);
		}
	protected:
		string m_one;
		string m_two;
		string m_location;
	};


	class string_length_too_long : public basic_error<codes> {
	public:
		string_length_too_long(code_error_objs::code_obj& code,std::size_t length, const string& location = get_location()) noexcept
			:basic_error<codes>(code.output()),m_length(length), m_location(location) {
		}

		string full_error_message() noexcept override {
			return std::format(READ_ONLY_STRING("error message: {}\n, location: {}\n, string length: {}"), m_error_code.m_two, m_location, m_length);
		}
	protected:
		std::size_t m_length;
		string m_location;
	};


	class index_out_of_range : public basic_error<codes>{
	public:
		index_out_of_range(code_error_objs::code_obj& code,size_t index, const string& location = errors::get_location()) noexcept
			:basic_error<codes>(code.output()), m_index(index), m_location(location){ }

		string full_error_message() noexcept override {
			return std::format(READ_ONLY_STRING("error message: {}\n, location: {}\n, index: {}\n"), m_error_code.m_two, m_location,m_index );
		}
	protected:
		size_t m_index;
		string m_location;
	};


	class dx_error : public basic_error<codes>{
	public:
		dx_error(code_error_objs::code_obj& code,HRESULT hr, const string& location = errors::get_location()) noexcept;
		dx_error(code_error_objs::code_obj& code,HRESULT hr, ID3DBlob* error, const string& location = errors::get_location()) noexcept;
		
		// only use if dx11 device was created
		dx_error(code_error_objs::code_obj& code,HRESULT hr, ID3D11InfoQueue* debug_info_p, const string& location = errors::get_location()) noexcept;

		string full_error_message() noexcept override {
			return std::format(READ_ONLY_STRING("error message: {}\n, location: {}\n, hr: {}\n, directx error: {}\n, directx debug info: {}"), m_error_code.m_two, m_location, m_hr_str, m_dx_error_blob_str,m_debug_info_str);
		}
	private:
		string m_location;
		string m_dx_error_blob_str;
		string m_debug_info_str;
		string m_hr_str;

		void translate_HRESULT(HRESULT hr) noexcept;
		void translate_error_blob(ID3DBlob* error) noexcept;
		void translate_debug_info(ID3D11InfoQueue* debug_info_p) noexcept;
	};


	class to_wide_string_fail : public basic_error<codes> {
	public:
		to_wide_string_fail(code_error_objs::code_obj& code, const string& location = errors::get_location()) noexcept
			:basic_error<codes>(code.output()),m_location(location){ }
	protected:
		string m_location;
	};

	class division_by_zero : public basic_error<codes> {
	public:
		division_by_zero(code_error_objs::code_obj& code, const string& location = errors::get_location()) noexcept
			:basic_error<codes>(code.output()), m_location(location) { }
	protected:
		string m_location;
	};

	class to_narrow_string_fail : public basic_error<codes> {
	public:
		to_narrow_string_fail(code_error_objs::code_obj& code, const string& location = errors::get_location()) noexcept
			:basic_error<codes>(code.output()), m_location(location) {}
	protected:
		string m_location;
	};

	class filesystem_error : public basic_error<codes> {
	public:
		filesystem_error(code_error_objs::code_obj& code,std::error_code ec, const string& location = errors::get_location()) noexcept
			:basic_error<codes>(code.output()),m_ec(ec), m_location(location) {}

		string full_error_message() noexcept override {
			std::string error_message = m_ec.message();
#if USING_WIDE_STRINGS
			std::wstring wide_error_message = to_wide_string(error_message);
			return std::format(READ_ONLY_STRING("error message: {}\n filesystem error code: {} \n"), m_error_code.m_two, wide_error_message);
#endif
		
#if USING_NARROW_STRINGS
			return std::format(READ_ONLY_STRING("error message: {}\n filesystem error code: {} \n"), m_error_code.m_two, error_message);
#endif
		}
	protected:
		string m_location;
		std::error_code m_ec;
	};
}

/*  some ways of dealing with exceptions and errors	*/
/****************************************************/
namespace errors {
	template<typename function_return_type, typename ... function_arguments_types>
	inline auto handle_exceptions(
		std::function<function_return_type(function_arguments_types ...)> function_that_may_throw,
		function_arguments_types ... individual_arguments) -> std::optional<function_return_type> {


		try {
			return function_that_may_throw(individual_arguments ...);
		}
		catch (const std::logic_error& e) {

		}
		catch (const std::runtime_error& e) {

		}
		catch (const std::exception& e) {

		}
		catch (const std::filesystem::filesystem_error& e) {

		}

		return std::nullopt;
	}

	void handle_basic_error_codes(codes code, const string& location = errors::get_location()) noexcept;
	void handle_win32_error_codes(win32_codes code, const string& location = errors::get_location()) noexcept;
}