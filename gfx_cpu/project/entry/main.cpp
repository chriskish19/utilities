/***********************************************
* File: main.cpp
*
* Purpose: main entry into the program
*
* Project: core
*
************************************************/

#pragma once
#include NAMES_INCLUDE
#include ALL_DEPENDS_INCLUDE_PATH
#include WINDOW_CLASS_INCLUDE_PATH
#include API_INCLUDE_PATH


int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	
	try {
		auto window = std::make_unique<core::drawing>();
		{
			core::codes code = window->load();
			core::output_code(code);
			if (code != core::codes::success) {
				return static_cast<int>(code);
			}
		}

		std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
		MSG msg = { 0 };
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				auto now = std::chrono::high_resolution_clock::now();
				float deltaTime = std::chrono::duration<float>(now - lastTime).count();
				lastTime = now;

				
				window->update(deltaTime);
				window->draw();
			}
		}
	}
	catch (const core::ce& e) {
		// core error exception
		core::output_ce(e);
		return static_cast<int>(e.m_code);
	}
	catch (const std::exception& e) {
		// exception from c++ standard lib
		core::output_se(e);
		return static_cast<int>(core::codes::std_exception_caught);
	}
	catch (...) {
		// any error
		core::output_code(core::codes::unknown_exception_caught);
		return static_cast<int>(core::codes::unknown_exception_caught);
	}

	return static_cast<int>(core::codes::success);
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