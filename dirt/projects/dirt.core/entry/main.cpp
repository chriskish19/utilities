/**********************************************************/
//
// File: main.cpp
//
// Purpose: main entry point
//
// Project: dirt.core
//
/**********************************************************/

#include CORE_NAMES_INCLUDE
#include CORE_STL_INCLUDE_PATH
#include CORE_ARGS_INCLUDE_PATH
#include CORE_API_INCLUDE_PATH


int main(int argc, char* argv[]) {
    // argc: argument count
    // argv: argument vector (array of C-strings)

    std::vector<std::string> args_v;
    for (int i = 0; i < argc; ++i) {
        args_v.push_back(argv[i]);
    }
    
    std::filesystem::path dirt_list_txt_path;
    bool next_is_path = false;
    std::vector<core::arg_pkg> found_args_v;
    bool dlist_path_is_valid = false;
    core::arg_entry cmdline_entry;
    cmdline_entry.entry_number = 1;
    core::args last_arg = core::args();

    // check the arguments for the dirt_lists.txt path
    for (const auto& s : args_v) {
        if (next_is_path == true) {
            dirt_list_txt_path = s;
            
            // check if path is valid
            dlist_path_is_valid = std::filesystem::exists(dirt_list_txt_path);

            if (dlist_path_is_valid == false) {
                auto error_code = core::match_code(core::codes::invalid_file_path);
                core::output_em(error_code);
                return static_cast<int>(error_code.m_code);
            }
            
            break;
        }

        auto found = core::gbl_args_mp.find(s);
        if (found != core::gbl_args_mp.end()) {
            auto pkg = core::match_arg_enum(found->second);
            if (pkg.m_arg == core::args::dirt_list_path) {
                next_is_path = true;
            }
            else if (pkg.m_arg == core::args::src) {
                // next arg should be the path
                last_arg = pkg.m_arg;
            }
            else if (pkg.m_arg == core::args::dst) {
                // next arg should be the path
                last_arg = pkg.m_arg;
            }

            found_args_v.push_back(pkg);
        }

        if (last_arg == core::args::src and std::filesystem::exists(s) == true) {
            cmdline_entry.src_p = s;
        }

        if (last_arg == core::args::dst and std::filesystem::exists(s) == true) {
            cmdline_entry.dst_p = s;
        }
    }

    std::vector<core::args> _args_v_ = core::arg_pkg_to_args(found_args_v);
    cmdline_entry.args_v = _args_v_;
    

    if (dlist_path_is_valid == true) {
        // read the list file and parse it, ignoring any other cmd args

        // parse the list file
        core::codes code;
        std::vector<core::arg_entry> entrys_v = core::parse_file(dirt_list_txt_path, &code);

        if (code != core::codes::success) {
            auto error_code = core::match_code(code);
            core::output_em(error_code);
            return static_cast<int>(error_code.m_code);
        }

        
    }
    else {
        bool cmdline_validated = core::validate_entry(cmdline_entry);

        if (cmdline_validated == false) {
            auto error_code = core::match_code(core::codes::invalid_cmd_args);
            core::output_em(error_code);
            return static_cast<int>(error_code.m_code);
        }

        // we have a group match found
        // now pass the args to the file processing loop
        ////.............................................
    }


    


    return static_cast<int>(core::codes::success);
}