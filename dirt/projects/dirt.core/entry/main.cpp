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
            found_args_v.push_back(pkg);
        }
    }

    if (dlist_path_is_valid == true) {
        // read the list file and parse it, ignoring any other cmd args

        // parse the list file
    }
    else {
        // validate the cmd args
        std::vector<core::args> validated_args_v = core::validate_args(found_args_v);

        if (validated_args_v.empty() == true) {
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