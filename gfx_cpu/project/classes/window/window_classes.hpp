/***********************************************
* File: window_classes.hpp
*
* Purpose: win32 window creation
*
* Project: core
*
************************************************/

#pragma once
#include NAMES_INCLUDE
#include ALL_DEPENDS_INCLUDE_PATH
#include CODES_INCLUDE_PATH
#include DESCRIPTION_INCLUDE_PATH
#include EXCEPT_OBJECT_INCLUDE_PATH
#include API_INCLUDE_PATH



#define MIN_WIDTH 100
#define MIN_HEIGHT 100
#define M_PI std::numbers::pi
#define PIXEL 3
#define CIRCLE_POINTS 256

namespace core {
	struct window_description {
		DWORD					dwExStyle;    // The extended window style of the window being created
		const_character_p		lpClassName;  // The class name of the window
		const_character_p		lpWindowName; // The name/title of the window
		DWORD					dwStyle;      // The style of the window
		int						X;            // The initial horizontal position of the window
		int						Y;            // The initial vertical position of the window
		int						nWidth;       // The width of the window
		int						nHeight;      // The height of the window
		HWND					hWndParent;   // Handle to the parent or owner window
		HMENU					hMenu;        // Handle to a menu, or child-window identifier
		HINSTANCE				hInstance;    // Handle to the instance of the module to be associated with the window
		LPVOID					lpParam;      // Pointer to additional application-defined data
	};

	// create a window from a window description
	HWND create_window(const window_description& wd);

	// register a window class
	codes register_window(const WNDCLASSEX& wc);




	class standard_window {
	public:
		// window is default init
		standard_window() = default;

		// static window procedure
		static LRESULT CALLBACK s_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		// initialize the window
		codes load();

		HWND handle() { return m_handle; }
		UINT width();
		UINT height();

	protected:
		// window handle
		HWND m_handle = nullptr;

		// class member function window procedure
		LRESULT CALLBACK this_window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


		window_description m_wd = {
			NULL,
			ROS("StandardWindow"),
			ROS("gfx_cpu.core"),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			NULL,
			NULL,
			GetModuleHandle(NULL),
			this
		};


		WNDCLASSEX m_wc = {
		   sizeof(WNDCLASSEX),
		   CS_HREDRAW | CS_VREDRAW,
		   s_window_proc,
		   0,
		   0,
		   GetModuleHandle(NULL),
		   LoadIcon(nullptr, IDI_APPLICATION),
		   LoadCursor(nullptr, IDC_ARROW),
		   reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
		   nullptr,
		   ROS("StandardWindow"),
		   LoadIcon(nullptr, IDI_APPLICATION)
		};
	};

	struct FourierComponent {
		float freq;
		float amplitude;
		float phase;
		std::complex<float> coeff;
	};

	class drawing : public standard_window{
	public:
		drawing();

		void draw();
		void update(float dt);
	protected:
		std::vector<uint8_t> m_frame_buffer_v = {}; // RGB pixel buffer

		void put_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);

		float m_accum_time = 0;

		void line(int x0, int y0, int x1, int y1, uint8_t r, uint8_t g, uint8_t b);

		std::vector<FourierComponent> compute_dft(const std::vector<std::complex<float>>& input);
		std::vector<std::complex<float>> generate_circle(int numPoints, float radius = 100.0f);
		std::vector<std::complex<float>> generate_bicycle_path();
		std::vector<std::complex<float>> generate_koch_snowflake(int level = 3);
		void subdivide_koch(std::vector<std::complex<float>>& path,int level);

		std::complex<float> draw_epicycles(
			float time,
			const std::vector<FourierComponent>& fourier,
			int cx, int cy
		);

		std::vector<FourierComponent> fourier;
		std::vector<std::complex<float>> trail;

		void fill_buffer(std::size_t size);
		void resize_draw_window(UINT new_width, UINT new_height);

		UINT m_draw_width = 800;
		UINT m_draw_height = 600;
	};
}