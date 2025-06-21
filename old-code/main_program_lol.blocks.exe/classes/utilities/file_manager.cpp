#include "file_manager.hpp"

utilities::file_manager::file_manager(const std::filesystem::path& file_path)
	:m_file_path(file_path)
{

}

errors::codes utilities::file_manager::open(std::ios_base::openmode mode)
{
	std::error_code ec = std::error_code();
	bool is_valid = false;
	is_valid = std::filesystem::exists(m_file_path, ec);

	if (is_valid == false) {
#if ENABLE_ALL_EXCEPTIONS
		{
			code_error_objs::code_obj error(code_error_objs::filesystem_error);
			throw errors::filesystem_error(error, ec);
		}
#endif

#if ENABLE_ERROR_OUTPUT_WINDOW
		{
			code_error_objs::code_obj error(code_error_objs::filesystem_error);
			errors::filesystem_error fs_error(error, ec);
			errors::show_error_message_window(fs_error.full_error_message());
		}
#endif

	}
	else {
		m_file_stream.open(m_file_path, mode);

		// Todo: finish this
		if (m_file_stream.is_open() == false) {
			std::ios_base::iostate stream_state = m_file_stream.rdstate();
			switch (stream_state) {
			case std::ios_base::goodbit:
				// return a code here
				break;
			case std::ios_base::badbit:
				break;
			case std::ios_base::failbit:
				break;
			case std::ios_base::eofbit:
				break;
			default:
				break;
			}
		}
	}
	
	return errors::codes(errors::codes::success);
}

utilities::string utilities::file_manager::file_data_to_string()
{
	ostringstream buffer;
	buffer << m_file_stream.rdbuf();	// Read entire file into stream
	return string(buffer.str());

}
