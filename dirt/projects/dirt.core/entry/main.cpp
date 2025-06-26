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

using namespace core;
int main(int argc, char* argv[]) {
    cmdline args(argc,argv);
    {
        codes code = args.parse();
        if (code != codes::success) {
            auto error_code = match_code(code);
            output_em(error_code);
            return static_cast<int>(code);
        }
    }

    {
        codes code = args.validate();
        if (code != codes::success) {
            auto error_code = match_code(code);
            output_em(error_code);
            return static_cast<int>(code);
        }
    }



    


    return static_cast<int>(core::codes::success);
}