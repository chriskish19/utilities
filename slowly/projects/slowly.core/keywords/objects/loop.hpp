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




namespace core {
    class loop {
    public:
        loop(const std::string& data);

        codes parse();

        std::string to_cpp();

    protected:
        std::string m_data;

        // replace any with variables
        std::string m_template = {
            "loop( any : any ) { }"
        };

        std::string m_main_arg;

        std::string m_cpp_template = {
            "for (auto& i : array){ }"
        };
    };
}
