/****************************************************************
	Header: dx_device_init.hpp

	Purpose: Create a direct x device.

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

namespace dx {
	class devices_11 {
	public:
		devices_11(UINT window_width, UINT window_height, HWND window_handle, const string& window_name);
		~devices_11();

		IDXGISwapChain* get_swap_p() { return m_sc_p; }
		ID3D11Device* get_device_p() { return m_device_p; }
		ID3D11DeviceContext* get_context_p() { return m_device_context_p; }
	private:
		errors::codes create_device();
	protected:
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
	};
}