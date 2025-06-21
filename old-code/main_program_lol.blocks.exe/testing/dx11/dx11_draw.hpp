/****************************************************************
	Header: draw.hpp

	Purpose: this header is to test drawing shapes in directx 11

****************************************************************/

#pragma once

// all macros
#include "main_program_lol.blocks.exe/macros/include/lol.macros.include.hpp"

// all external library headers
#include "main_program_lol.blocks.exe/pch/includes/external.hpp"

// error codes
#include "main_program_lol.blocks.exe/errors/lol.codes.hpp"

// globals
#include "main_program_lol.blocks.exe/global/globals.hpp"

// dx dependencies
#include "main_program_lol.blocks.exe/dx/dx_device_init.hpp"

// math
#include "main_program_lol.blocks.exe\classes\math\lol.math.hpp"

namespace testing {
	class triangle{
	public:
		triangle(UINT window_width, UINT window_height, HWND window_handle, const string& window_name);
		~triangle();

		void render();
	protected:
		IDXGIAdapter* m_adaptor_p = nullptr;
		IDXGISwapChain* m_sc_p = nullptr;
		ID3D11Device* m_device_p = nullptr;
		ID3D11DeviceContext* m_device_context_p = nullptr;
		const D3D_FEATURE_LEVEL* m_feature_levels = nullptr;
		size_t m_feature_levels_count = 6;
		const DXGI_SWAP_CHAIN_DESC* m_swap_chain_desc_p = nullptr;
		HWND m_window_handle = nullptr;
		ID3D11RenderTargetView* m_render_target_p = nullptr;
		string m_window_name;
#if ENABLE_DX_DEBUG
		ID3D11InfoQueue* m_dx_debug_info_p = nullptr;
#endif

		errors::codes compile();

		ID3DBlob* m_vs_code_p = nullptr;
		ID3DBlob* m_ps_code_p = nullptr;

		class vertex {
		public:
			float x;
			float y;
		};

		const vertex m_vertices[3]{
			{0.0f, 0.5f}, // top
			{0.5f,-0.5f}, // right
			{-0.5f,-0.5f} // left
		};
	};
}