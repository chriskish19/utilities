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
#include CORE_CMDLINE_INCLUDE_PATH
#include CORE_PROCESSOR_INCLUDE_PATH


using namespace core;
int main(int argc, char* argv[]) {
    std::unique_ptr<cmdline> p_args = std::make_unique<cmdline>(argc,argv);
    {
        codes code = p_args->parse();
        if (code != codes::success) {
            auto error_code = match_code(code);
            output_em(error_code);
            return static_cast<int>(code);
        }
    }

    {
        codes code = p_args->validate();
        if (code != codes::success) {
            auto error_code = match_code(code);
            output_em(error_code);
            return static_cast<int>(code);
        }
    }

    std::unique_ptr<process> pfile_processor = std::make_unique<process>(p_args->m_entrys_v);
    {
        codes code = pfile_processor->process_entry();
        if (code != codes::success) {
            auto error_code = match_code(code);
            output_em(error_code);
            return static_cast<int>(code);
        }
    }
    
    {
        codes code = pfile_processor->watch();
        if (code != codes::success) {
            auto error_code = match_code(code);
            output_em(error_code);
            return static_cast<int>(code);
        }
    }


    return static_cast<int>(core::codes::success);
}