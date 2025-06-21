/**********************************************************/
//
// File: codes.hpp
//
// Purpose: returnable error codes
// 
// Project: slowly.core
//
/**********************************************************/

#pragma once
#include CORE_NAMES_INCLUDE
#include CORE_STL_INCLUDE_PATH

namespace core {
    enum class codes {
        success = 0,
        uninit,
        invalid_file_path,
        syntax_mistake,
        file_open_fail,
    };


}
