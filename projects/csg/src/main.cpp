#include NAMES_INCLUDE
#include PCH_INCLUDE_PATH


bool is_all_caps(const std::string& s);
bool digit_begin(const std::string& s);
std::string to_all_lower(const std::string& s);
bool is_message(const std::string& s);

int main() {
    std::filesystem::path test_file(TEST_FILE_PATH);

    std::ifstream input_file(test_file);

    if (!input_file.is_open()) {
        std::cerr << "Failed to open file: " << test_file << std::endl;
        return 1;
    }

    std::ofstream output_file(test_file.parent_path() / "output.txt");
    
    std::string line,macro,message;
    while (std::getline(input_file, line)) {
        if (digit_begin(line) or line.empty()) {
            // skip to the next line
            // its a code
            continue;
        }
        else if (is_all_caps(line)) {
            // macro
            macro = line;
        }
        else if (is_message(line)){
            // message 
            message = line;

            // reconstruct
            output_file << "inline const character* " << to_all_lower(macro) << "_description = ROS(\""
                << message << "\");" << "\n";
        }

    }

    
    return 0;
}

bool is_all_caps(const std::string& s) {
    std::size_t count = 0;
    for (auto c : s) {
        if (std::isupper(c) or c == '_') {
            count++;
        }
        else {
            break;
        }
    }

    return count == s.size();
}

bool digit_begin(const std::string& s)
{
    if (s.empty() == false) {
        return std::isdigit(s[0]);
    }
    return false;
}

std::string to_all_lower(const std::string& s)
{
    std::string result;
    for (auto c : s) {
        char lower_c = std::tolower(c);
        result += lower_c;
    }
    return std::string(result);
}

bool is_message(const std::string& s)
{
    std::size_t upper_count = 0, lower_count = 0, alpha_count = 0;
    for (unsigned char c : s) {
        if (std::isalpha(c)) {
            alpha_count++;
        }

        if (std::islower(c)) {
            lower_count++;
        }

        if (std::isupper(c)) {
            upper_count++;
        }
    }

    return alpha_count > upper_count and upper_count < lower_count;
}
