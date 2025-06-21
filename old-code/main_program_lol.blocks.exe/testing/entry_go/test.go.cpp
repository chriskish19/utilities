#include "test.go.hpp"

errors::codes testing::basic_tests() {
	
	{
		std::filesystem::path exe_path = std::filesystem::current_path();
		errors::codes code = string_conversions_file(exe_path / "test_files/string.txt");
		testing::handle_basic_error_codes(code);
	}

	{
		errors::codes code = string_conversions("test string!");
		testing::handle_basic_error_codes(code);
	}

	{
		errors::codes code = create_windows(5);
		testing::handle_basic_error_codes(code);
	}

	{
		errors::codes code = window_logger();
		testing::handle_basic_error_codes(code);
	}


	return errors::codes(errors::codes::success);
}

errors::codes testing::draw_tests()
{
	{
		errors::codes code = dx11_draw();
		testing::handle_basic_error_codes(code);
	}
	
	return errors::codes::success;
}
