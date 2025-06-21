#include "dx11_draw.hpp"

testing::triangle::triangle(UINT window_width, UINT window_height, HWND window_handle, const string& window_name)
	:m_window_handle(window_handle),m_window_name(window_name)
{
	// TODO:
	// create structure descriptors
	// create device and swap chain
	// create vs shader pointer
	// create ps shader pointer

#if ENABLE_ALL_EXCEPTIONS
	if (m_window_handle == nullptr) {
		code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
		throw errors::pointer_is_nullptr(error, READ_ONLY_STRING("m_window_handle"));
	}
#endif

	m_feature_levels = new D3D_FEATURE_LEVEL[]{
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_11_0
	};

	/*
	
			typedef struct DXGI_MODE_DESC {
		  UINT                     Width;
		  UINT                     Height;
		  DXGI_RATIONAL            RefreshRate;
		  DXGI_FORMAT              Format;
		  DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
		  DXGI_MODE_SCALING        Scaling;
		} DXGI_MODE_DESC;

	*/

	DXGI_MODE_DESC mode_desc{
		window_width,
		window_height,
		DXGI_RATIONAL{0u,0u},
		DXGI_FORMAT_R8G8B8A8_UINT,
		DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		DXGI_MODE_SCALING_STRETCHED
	};

	/*
	
	typedef struct DXGI_SWAP_CHAIN_DESC {
		  DXGI_MODE_DESC   BufferDesc;
		  DXGI_SAMPLE_DESC SampleDesc;
		  DXGI_USAGE       BufferUsage;
		  UINT             BufferCount;
		  HWND             OutputWindow;
		  BOOL             Windowed;
		  DXGI_SWAP_EFFECT SwapEffect;
		  UINT             Flags;
		} DXGI_SWAP_CHAIN_DESC;
	
	*/

	DXGI_SWAP_CHAIN_DESC swap_desc{
		mode_desc,
		DXGI_SAMPLE_DESC{1,0},
		DXGI_USAGE_RENDER_TARGET_OUTPUT,
		1u,
		m_window_handle,
		TRUE,
		DXGI_SWAP_EFFECT_FLIP_DISCARD,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

	// find a good adaptor
	UINT i = 0;
	IDXGIAdapter* pAdapter;
	std::vector <IDXGIAdapter*> vAdapters;
	while (pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		vAdapters.push_back(pAdapter);
		++i;
	}




	// create device

	/*
	
	HRESULT D3D11CreateDeviceAndSwapChain(
		  [in, optional]  IDXGIAdapter               *pAdapter,
						  D3D_DRIVER_TYPE            DriverType,
						  HMODULE                    Software,
						  UINT                       Flags,
		  [in, optional]  const D3D_FEATURE_LEVEL    *pFeatureLevels,
						  UINT                       FeatureLevels,
						  UINT                       SDKVersion,
		  [in, optional]  const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
		  [out, optional] IDXGISwapChain             **ppSwapChain,
		  [out, optional] ID3D11Device               **ppDevice,
		  [out, optional] D3D_FEATURE_LEVEL          *pFeatureLevel,
		  [out, optional] ID3D11DeviceContext        **ppImmediateContext
		);
	
	*/

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
					m_adaptor_p,
					
	);
}

testing::triangle::~triangle()
{
	if (m_feature_levels != nullptr) {
		delete[] m_feature_levels;
	}

	if (m_swap_chain_desc_p != nullptr) {
		delete m_swap_chain_desc_p;
	}

	if (m_device_context_p != nullptr) {
		m_device_context_p->Release();
	}

	if (m_device_p != nullptr) {
		m_device_p->Release();
	}

	if (m_sc_p != nullptr) {
		m_sc_p->Release();
	}

	if (m_render_target_p != nullptr) {
		m_render_target_p->Release();
	}

	if (m_dx_debug_info_p != nullptr) {
		m_dx_debug_info_p->Release();
	}
	
	if (m_vs_code_p != nullptr) {
		m_vs_code_p->Release();
	}

	if (m_ps_code_p != nullptr) {
		m_ps_code_p->Release();
	}
}

void testing::triangle::render()
{



}

errors::codes testing::triangle::compile()
{
	std::filesystem::path exe_path = std::filesystem::current_path();

	std::filesystem::path vs_path = exe_path / "shaders/vertex_shader.hlsl";
	std::filesystem::path ps_path = exe_path / "shaders/pixel_shader.hlsl";



	/*
	HRESULT D3DCompileFromFile(
			  [in]            LPCWSTR                pFileName,
			  [in, optional]  const D3D_SHADER_MACRO *pDefines,
			  [in, optional]  ID3DInclude            *pInclude,
			  [in]            LPCSTR                 pEntrypoint,
			  [in]            LPCSTR                 pTarget,
			  [in]            UINT                   Flags1,
			  [in]            UINT                   Flags2,
			  [out]           ID3DBlob               **ppCode,
			  [out, optional] ID3DBlob               **ppErrorMsgs
			);
	*/
	{
		ID3DBlob* local_error_p = nullptr;

		HRESULT hr = D3DCompileFromFile(
			vs_path.c_str(),				// file path
			NULL,							// not used
			NULL,
			"main",							// entry point name in shader code
			"vs_5_0",						// version of shader code
			D3DCOMPILE_DEBUG,				// flags for compiling
			NULL,							// not used
			&m_vs_code_p,					// vertex shader byte code
			&local_error_p
		);

		if (FAILED(hr)) {
			code_error_objs::code_obj local_error_info(code_error_objs::dx_error);
			errors::dx_error dx_error_class(local_error_info, hr, local_error_p);
			errors::show_error_message_window(dx_error_class.full_error_message());
		}

		if (local_error_p != nullptr) {
			local_error_p->Release();
		}

		return errors::codes::dx_error;
	}
	


	{
		ID3DBlob* local_error_p = nullptr;

		HRESULT hr = D3DCompileFromFile(
			vs_path.c_str(),				// file path
			NULL,							// not used
			NULL,
			"main",							// entry point name in shader code
			"ps_5_0",						// version of shader code
			D3DCOMPILE_DEBUG,				// flags for compiling
			NULL,							// not used
			&m_ps_code_p,
			&local_error_p
		);

		if (FAILED(hr)) {
			code_error_objs::code_obj local_error_info(code_error_objs::dx_error);
			errors::dx_error dx_error_class(local_error_info, hr, local_error_p);
			errors::show_error_message_window(dx_error_class.full_error_message());
		}

		if (local_error_p != nullptr) {
			local_error_p->Release();
		}

		return errors::codes::dx_error;
	}

	return errors::codes::test_success;
}
