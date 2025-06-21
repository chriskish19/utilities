#include "main_program_lol.blocks.exe/pch/pch.hpp"

// set in "debug_settings.hpp" check "pch.hpp"
#if USE_MAIN_ENTRY_CPP

/*  unicode is defined with cmake!
#ifndef UNICODE
#define UNICODE 
#endif // !UNICODE
*/

// handles most running of program
#include "main_program_lol.blocks.exe/entry/includes/go.hpp"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	main::initialize();
	main::go();
	main::shutdown();

	// win32 api specific return code
	return NULL;
}


// USE TEST API ENTRY POINT
#elif USE_TEST_API

#include "main_program_lol.blocks.exe/testing/api/test.api.hpp"
#include "main_program_lol.blocks.exe\testing\entry_go\test.go.hpp"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {

	// run test api here...
	testing::draw_tests();

	COUT << ROS("All Tests Completed. Press enter to exit... \n");
	return NULL;
}

int main() {
	// Prepare arguments for wWinMain
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	HINSTANCE hPrevInstance = nullptr;
	LPWSTR lpCmdLine = GetCommandLineW();
	int nShowCmd = SW_SHOW;

	// Call wWinMain
	return wWinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}



// USE_MAIN_ENTRY_CPP
#endif
