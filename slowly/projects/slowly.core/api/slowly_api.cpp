
/**********************************************************/
//
// File: slowly_api.cpp
//
// Purpose: helper functions definitons
//
// Project: slowly.core
//
/**********************************************************/

#include CORE_NAMES_INCLUDE
#include CORE_API_INCLUDE_PATH

core::slowly_package core::match_in_slowly(keywords key)
{
    switch (key) {
    case keywords::loop:
        return loop_package;
    case keywords::main:
        return main_package;
    case keywords::array:
        return array_package;
    case keywords::auto_:
        return auto_package;
    case keywords::print:
        return print_package;
    case keywords::open_parenthesis:
        return open_parenthesis_package;
    case keywords::closed_parenthesis:
        return closed_parenthesis_package;
    case keywords::open_bracket:
        return open_bracket_package;
    case keywords::closed_bracket:
        return closed_bracket_package;
    case keywords::semi_colon:
        return semi_colon_package;
    case keywords::comma:
        return comma_package;
    case keywords::backslash:
        return backslash_package;
    case keywords::single_quote:
        return single_quote_package;
    case keywords::double_foward_slash:
        return double_foward_slash_package;
    default:
        return unknown_package;
    }
}

core::slowly_package core::match_in_slowly(const std::string& name)
{
    if (loop_package.m_name == name) {
        return loop_package;
    }
    if (main_package.m_name == name) {
        return main_package;
    }
    if (array_package.m_name == name) {
        return array_package;
    }
    if (auto_package.m_name == name) {
        return auto_package;
    }
    if (print_package.m_name == name) {
        return print_package;
    }
    if (open_parenthesis_package.m_name == name) {
        return open_parenthesis_package;
    }
    if (closed_parenthesis_package.m_name == name) {
        return closed_parenthesis_package;
    }
    if (open_bracket_package.m_name == name) {
        return open_bracket_package;
    }
    if (closed_bracket_package.m_name == name) {
        return closed_bracket_package;
    }
    if (semi_colon_package.m_name == name) {
        return semi_colon_package;
    }
    if (comma_package.m_name == name) {
        return comma_package;
    }
    if (backslash_package.m_name == name) {
        return backslash_package;
    }
    if (single_quote_package.m_name == name) {
        return single_quote_package;
    }
    if (double_foward_slash_package.m_name == name) {
        return double_foward_slash_package;
    }
    // Return a default package or throw if invalid
    return unknown_package;
}

bool core::is_keyword(const std::string& keyword, const std::vector<std::string>& container)
{
    auto found = std::find(container.begin(), container.end(), keyword);
    return found != container.end();
}

std::vector<std::string> core::add_tokens(const std::filesystem::path& file, core::codes* code)
{
    // check if file exists
    if (std::filesystem::exists(file) == false) {
        *code = codes::invalid_file_path;
        // empty vector
        return {};
    }

    // open the file for reading
    std::ifstream code_file(file);

    if (code_file.is_open() == false) {
        *code = codes::file_open_fail;
        // empty vector
        return {};
    }

    std::vector<std::string> tokens_v;
    std::string line;
    std::size_t last = 0;
    std::size_t first = 0;
    std::string token;
    while (std::getline(code_file, line)) {
        first = line.find_first_not_of(" ");
        last = line.find_first_of(" ", first);
        if (last != std::string::npos or first != std::string::npos) {
            token = line.substr(first, last);
            if (match_in_slowly(token) != unknown_package) {
                tokens_v.push_back(token);
            }
        }


        while (last != std::string::npos or first != std::string::npos) {
            first = line.find_first_not_of(" ");
            last = line.find_first_of(" ", first);
            last--;
            token = line.substr(first, last);

            if (match_in_slowly(token) != unknown_package) {
                tokens_v.push_back(token);
            }
        }
    }

    *code = core::codes::success;
    return tokens_v;
}

core::codes core::translate(std::filesystem::path sl_file)
{
    // check if file exists
    if (std::filesystem::exists(sl_file) == false) {
        return codes::invalid_file_path;
    }


}
