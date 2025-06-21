/****************************************************************
	Header: windows_includes.hpp

	Purpose: includes windows headers for using win32 api.

****************************************************************/


#ifndef WINDOWS_INCLUDES_HPP
#define WINDOWS_INCLUDES_HPP

// windows header
#include <windows.h>

// comptr header
#include <wrl.h>

// hr result translation
#include <comdef.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#endif