#include "windows_includes.hpp"

// win32 api defines min macro that conflicts with c++ stl
#if defined(min)
#undef min
#endif

// win32 api defines max macro that conflicts with c++ stl
#if defined(max)
#undef max
#endif