#include "file_logger.hpp"

utilities::file_logs::file_logs()
{
	m_log_file_path = std::filesystem::current_path() / m_file_name;
}

utilities::file_logs::file_logs(const std::filesystem::path& log_file_path)
{
	bool dir_created = false;
	
	if (std::filesystem::exists(log_file_path) == false) {
		dir_created = std::filesystem::create_directories(log_file_path);
	}
	
	if (dir_created == true) {

	}

	m_log_file_path = log_file_path / m_file_name;
}

errors::codes utilities::file_logs::flush()
{
	return errors::codes();
}

errors::codes utilities::file_logs::write_log_message(const string& message)
{
	return errors::codes();
}

errors::codes utilities::file_logs::create_log_file()
{
	std::fstream log_file(m_log_file_path);
	
	
	return errors::codes();
}
