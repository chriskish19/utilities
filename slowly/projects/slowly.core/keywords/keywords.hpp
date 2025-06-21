/**********************************************************/
//
// File:
//
// Purpose:
//
// Project:
//
/**********************************************************/

#pragma once
#include CORE_NAMES_INCLUDE
#include CORE_STL_INCLUDE_PATH


namespace core {
    enum class keywords {
        loop,
        unknown,
        main,
        array,
        auto_,
        print,
        open_parenthesis,
        closed_parenthesis,
        open_bracket,
        closed_bracket,
        semi_colon,
        comma,
        backslash,
        single_quote,
        double_foward_slash,
    };

    class slowly_package {
    public:
        slowly_package(keywords key, const std::string& name)
            :m_key(key), m_name(name) {
        }

        const keywords m_key;
        const std::string m_name;

        bool operator==(const slowly_package& p) {
            return this->m_key == p.m_key;
        }
    };

    // list all the keyword objects
    inline const slowly_package unknown_package(keywords::unknown, "unknown key");
    inline const slowly_package loop_package(keywords::loop, "loop");
    inline const slowly_package main_package(keywords::main, "main");
    inline const slowly_package array_package(keywords::array, "array");
    inline const slowly_package auto_package(keywords::auto_, "auto");
    inline const slowly_package print_package(keywords::print, "print");
    inline const slowly_package open_parenthesis_package(keywords::open_parenthesis, "(");
    inline const slowly_package closed_parenthesis_package(keywords::closed_parenthesis, ")");
    inline const slowly_package open_bracket_package(keywords::open_bracket, "[");
    inline const slowly_package closed_bracket_package(keywords::closed_bracket, "]");
    inline const slowly_package semi_colon_package(keywords::semi_colon, ";");
    inline const slowly_package comma_package(keywords::comma, ",");
    inline const slowly_package backslash_package(keywords::backslash, "\\");
    inline const slowly_package single_quote_package(keywords::single_quote, "'");
    inline const slowly_package double_foward_slash_package(keywords::double_foward_slash, "//");
}