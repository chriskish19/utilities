/***********************************************
* File: windows_includes.hpp
*
* Purpose: include windows headers
*
*
*
************************************************/

#pragma once

// main windows header
#include <windows.h>

// common controls library
#include <commctrl.h>

// virtual hard disk library
#include <virtdisk.h>

// The userenv.h header file is part of the Windows API and provides functions for managing user profiles. 
// These functions allow applications to load, unload, and query user profiles, 
// which are essential for accessing user-specific settings and data.
#include <userenv.h>

// hresult errors
#include <comdef.h>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

