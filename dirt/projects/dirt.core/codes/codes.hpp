/**********************************************************/
//
// File: codes.hpp
//
// Purpose: returnable error codes
//
// Project: dirt.core
//
/**********************************************************/

#pragma once

namespace core {
    enum class codes {
        success = 0,
        uninit,
        invalid_file_path,
        syntax_mistake,
        file_open_fail,
        unknown,
        invalid_cmd_args,
        invalid_io_handle,
        handle_nullptr,
        read_dir_changes_fail,
        no_valid_entries,
    };

    class code_pkg {
    public:
        code_pkg(codes code, std::string s_code)
            :m_code(code),m_s_code(s_code){ }

        const codes m_code;
        const std::string m_s_code;
    };

    inline const code_pkg success_pkg(codes::success, "core::codes::success | message: successful execution.");
    inline const code_pkg c_uninit_pkg(codes::uninit, "core::codes::uninit | message: codes is uninitialized or default constructed.");
    inline const code_pkg invalid_file_path_pkg(codes::invalid_file_path, "core::codes::invalid_file_path | message: file path given does not exist on the system.");
    inline const code_pkg syntax_mistake_pkg(codes::syntax_mistake, "core::codes::syntax_mistake | message: syntax error in the given code.");
    inline const code_pkg file_open_fail_pkg(codes::file_open_fail, "core::codes::file_open_fail | message: error occurred while attempting to open a file.");
    inline const code_pkg c_unknown_pkg(codes::unknown, "core::codes::unknown | message: unknown error code.");
    inline const code_pkg invalid_cmd_args_pkg(codes::invalid_cmd_args, "core::codes::invalid_cmd_args | message: one or more command line arguments is invalid.");
    inline const code_pkg invalid_io_handle_pkg(codes::invalid_io_handle, "core::codes::invalid_io_handle | message: the handle was not created call the win32 function GetLastError() for more info.");
    inline const code_pkg handle_nullptr_pkg(codes::handle_nullptr, "core::codes::handle_nullptr | message: the windows win32 handle is nullptr,call the win32 function GetLastError() for more info.");
    inline const code_pkg read_dir_changes_fail_pkg(codes::read_dir_changes_fail, "core::codes::read_dir_changes_fail | message: the windows function ReadDirectoryChangesW() failed ,call the win32 function GetLastError() for more info.");
    inline const code_pkg no_valid_entries_pkg(codes::no_valid_entries, "core::codes::no_valid_entries | message: in the dirt_list.txt file given no valid entries were found, probably a syntax error.");
}