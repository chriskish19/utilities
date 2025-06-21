/****************************************************************
	Header: debug_settings.hpp

	Purpose: defines macros for controling the whole program

****************************************************************/


// just a nice readable thing
#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

// Control Debug and optimizations for lol.blocks main
// graphics rendering has its own debug macro
#define ENABLE_FULL_DEBUG ON
#if ENABLE_FULL_DEBUG
#define ENABLE_FULL_OPTIMIZATIONS OFF
#else
#define ENABLE_FULL_OPTIMIZATIONS ON
#endif

// Control debug for graphics api
#define ENABLE_DX_DEBUG ON

// Control logging
#define ENABLE_DEEP_LOGS ON
#define ENABLE_MED_LOGGING ON
#define ENABLE_MIN_LOGGING ON

// set which graphics api we are using
#define USING_DX12_API OFF
#define USING_DX11_API ON

// control exceptions
#define ENABLE_ALL_EXCEPTIONS ON

// output errors to a small window
#define ENABLE_ERROR_OUTPUT_WINDOW ON
