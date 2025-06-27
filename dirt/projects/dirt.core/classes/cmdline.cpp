#include "cmdline.hpp"
/**********************************************************/
//
// File: cmdline.cpp
//
// Purpose: cmdline.hpp definitions
//
// Project: dirt.core
//
/**********************************************************/

#include CORE_NAMES_INCLUDE
#include CORE_CMDLINE_INCLUDE_PATH

core::cmdline::cmdline(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i) {
        m_args_v.push_back(argv[i]);
    }
}

core::codes core::cmdline::parse()
{
    
    for (const auto& s : m_args_v) {
        if (m_next_is_dirt_path == true) {
            m_dirt_list_txt_path = s;
            m_dirt_lists_used = true;
            break;
        }


        // look for dirt_lists.txt
        auto found = core::gbl_args_mp.find(s);
        if (found != core::gbl_args_mp.end()) {
            if (found->second == core::args::dirt_list_path) {
                m_next_is_dirt_path = true;
            }
        }
    }


    // dirt_list_txt used
    if (m_dirt_lists_used == true) {
        if (std::filesystem::is_regular_file(m_dirt_list_txt_path) == false) {
            return codes::invalid_file_path;
        }

        // parse the list file
        core::codes code;
        m_entrys_v = core::parse_file(m_dirt_list_txt_path, &code);

        if (code != core::codes::success) {
            return code;
        }

    } // cmdline args : used one entry
    else {

        m_cmdline_entry.entry_number = 1;
        core::args last_arg = core::args();

        for (const auto& s : m_args_v) {
            
            auto found = core::gbl_args_mp.find(s);
            if (found != core::gbl_args_mp.end()) {
                if (found->second == core::args::src) {
                    // next arg should be the path
                    last_arg = found->second;
                }
                else if (found->second == core::args::dst) {
                    // next arg should be the path
                    last_arg = found->second;
                }
            }

            if (last_arg == core::args::src) {
                m_cmdline_entry.src_p = s;
            }

            if (last_arg == core::args::dst) {
                m_cmdline_entry.dst_p = s;
            }
        }

    }
    return codes::success;
}

core::codes core::cmdline::validate()
{
    if (m_dirt_lists_used == true) {
        
        std::vector<std::size_t> invalid_entrys_v;
        for (const auto& entry : m_entrys_v) {
            if (validate_entry(entry) == false) {
                std::cout << "error : invalid entry" << '\n';
                output_entry(entry);
                invalid_entrys_v.push_back(entry.entry_number);
            }
        }

        for (auto i : invalid_entrys_v) {
            m_entrys_v.erase(m_entrys_v.begin() + (i -1));
        }

        if (m_entrys_v.empty()) {
            return codes::no_valid_entries;
        }
    }
    else {
        if (validate_entry(m_cmdline_entry) == false) {
            std::cout << "error : invalid entry" << '\n';
            output_entry(m_cmdline_entry);
            return codes::no_valid_entries;
        }

        m_entrys_v.push_back(m_cmdline_entry);
    }
    return codes::success;
}
