/**********************************************************/
//
// File: loop.hpp
//
// Purpose: keyword loop object for when a loop is defined
//          in slowly
//
// Project: slowly.core
//
/**********************************************************/

#pragma once
#include CORE_NAMES_INCLUDE
#include CORE_STL_INCLUDE_PATH
#include CORE_CODES_INCLUDE_PATH
#include CORE_KEYWORDS_INCLUDE_PATH

namespace core {

    enum class loop_type {
        basic_for_loop,
        while_loop,
        for_each,
        nested_loop
    };


    class loop {
    public:
        loop(const std::string& code);
        codes parse();
        std::string to_cpp();
    protected:
        std::string extract_code(codes* code);
        loop_type find_type();
        
        codes build(loop_type loop);


        std::string m_data;
    };
}
