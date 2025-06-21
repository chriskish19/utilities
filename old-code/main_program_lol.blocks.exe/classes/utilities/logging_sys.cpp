#include "logging_sys.hpp"


utilities::logger::base_logger::~base_logger()
{
	if (m_log_message_p != nullptr) {
		delete m_log_message_p;
	}
	if (m_time_str_p != nullptr) {
		delete m_time_str_p;
	}
}

errors::codes utilities::logger::base_logger::log_a_message(const string& message)
{
	// we only allow the message length to be constant set with the static variable
	// reserved_length_mem_heap
	if (message.length() > log_message::reserved_length_mem_heap) {

#if ENABLE_ALL_EXCEPTIONS
		// if its too long we throw a object that handles this specific error
		code_error_objs::code_obj error(code_error_objs::string_length_too_long);
		throw errors::string_length_too_long(error,message.length());
		return errors::codes::exception_thrown_and_handled;
#endif

		// if exceptions are disabled in debug macros header
		return errors::codes::string_length_too_long;
	}
	
	{
		// time stamp time string
		errors::codes code = m_time_str_p->set_current_time();
		if (code != errors::codes::success) {
			return code;
		}
	}

	{
		// set actual log message with time stamp
		errors::codes code = m_log_message_p->set_log_message(m_time_str_p->m_time_str + message);
		if (code != errors::codes::success) {
			return code;
		}
	}
	
	return errors::codes::success;
}

utilities::logger::base_logger::time_str::time_str()
{
	// set reserved capacity for time string
	// its a constant time stamp anyways (always same length)
	m_time_str.reserve(reserved_length_heap_mem);
}

errors::codes utilities::logger::base_logger::time_str::set_current_time()
{
	// Get the current time as a time_point
	auto now = std::chrono::system_clock::now();

	// format it like this [2025-01-29 17:38:XXXXXX]
	m_time_str = std::format(READ_ONLY_STRING("[{}]"), now);

	// not much can be checked for errors here so...
	return errors::codes::success;
}

utilities::logger::base_logger::log_message::log_message()
{
	m_message.reserve(reserved_length_mem_heap);
}

errors::codes utilities::logger::base_logger::log_message::set_log_message(const string& message)
{
	// not much can go wrong here...
	m_message = message;
	return errors::codes::success;
}

utilities::logger::~logger()
{
	if (m_stored_logs_p->load() != nullptr) {
		auto actual_p = m_stored_logs_p->load();
		delete actual_p;
	}

	// safe to delete atomic
	if (m_stored_logs_p != nullptr) {
		delete m_stored_logs_p;
	}
}

utilities::logger::logs::logs()
{
	m_bl_vec_p.reserve(m_bl_vec_reserved_capacity);

	// allocate all the base_logs
	for (size_t i = 0; i < m_bl_vec_reserved_capacity; ++i) {
		base_logger* new_base_logger = new base_logger;

#if TESTING
#if USING_WIDE_STRINGS
		new_base_logger->log_a_message(READ_ONLY_STRING("Hello test log:" + std::to_wstring(i)));
#endif // USING_WIDE_STRINGS

#if USING_NARROW_STRINGS
		new_base_logger->log_a_message(READ_ONLY_STRING("Hello test log:" + std::to_string(i)));
#endif // USING_NARROW_STRINGS
#endif // TESTING

		m_bl_vec_p.push_back(new_base_logger);
	}
}

utilities::logger::logs::~logs()
{
	// deallocate all the base loggers
	for (auto p : m_bl_vec_p) {
		if (p != nullptr) {
			delete p;
		}
	}
}

errors::codes utilities::logger::logs::log_message(const string& message)
{
	std::lock_guard<std::mutex> local_lock(m_message_mtx);

	std::size_t max_index_position = m_bl_vec_p.size() - 1;

#if ENABLE_ALL_EXCEPTIONS
	if (m_index_pos > max_index_position) {
		code_error_objs::code_obj error(code_error_objs::index_out_of_range);
		throw errors::index_out_of_range(error,m_index_pos);
	}
#endif
	
	base_logger* bl_p = get_base_logger_p_by_index(m_index_pos);
	
#if ENABLE_ALL_EXCEPTIONS
	if (bl_p == nullptr) {
		code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
		throw errors::pointer_is_nullptr(error,READ_ONLY_STRING("base_logger* bl_p"));
	}
#endif

	if (m_index_pos < max_index_position) {
		m_index_pos++;
	}
	else {
		m_index_pos = 0;
	}

	return bl_p->log_a_message(message);
}

utilities::string utilities::logger::logs::get_most_recent_log()
{
	base_logger* last_element = m_bl_vec_p.back();
	base_logger::log_message* lm_p = last_element->get_message_p();
#if ENABLE_ALL_EXCEPTIONS
	if (last_element == nullptr) {
		code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
		throw errors::pointer_is_nullptr(error,READ_ONLY_STRING("base_logger* last_element"));
	}
	if (lm_p == nullptr) {
		code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
		throw errors::pointer_is_nullptr(error,READ_ONLY_STRING("base_logger::log_message* lm_p"));
	}
#endif

#if ENABLE_MED_LOGGING
	if (lm_p->m_message.empty()) {
		return READ_ONLY_STRING("most recent log is empty...");
	}
#endif
	
	return lm_p->m_message;
}

utilities::string utilities::logger::logs::get_log_by_index(size_t index)
{
#if ENABLE_ALL_EXCEPTIONS
	if (index > m_bl_vec_p.size()) {
		code_error_objs::code_obj error(code_error_objs::index_out_of_range);
		throw errors::index_out_of_range(error,index);
	}
#endif

	base_logger* elm = m_bl_vec_p.at(index);
	base_logger::log_message* lm_p = elm->get_message_p();

#if ENABLE_ALL_EXCEPTIONS
	if (elm == nullptr) {
		code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
		throw errors::pointer_is_nullptr(error,READ_ONLY_STRING("base_logger* elm"));
	}
	if (lm_p == nullptr) {
		code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
		throw errors::pointer_is_nullptr(error,READ_ONLY_STRING("base_logger::log_message* lm_p"));
	}
#endif

#if ENABLE_MED_LOGGING
	if (lm_p->m_message.empty()) {
		return std::format(READ_ONLY_STRING("the log at index {} is empty..."),index);
	}
#endif

	return lm_p->m_message;
}

utilities::logger::base_logger* utilities::logger::logs::get_base_logger_p_by_index(size_t index)
{
#if ENABLE_ALL_EXCEPTIONS
	if (index > m_bl_vec_p.size()) {
		code_error_objs::code_obj error(code_error_objs::index_out_of_range);
		throw errors::index_out_of_range(error,index);
	}
#endif

	base_logger* elm = m_bl_vec_p.at(index);
	
#if ENABLE_ALL_EXCEPTIONS
	if (elm == nullptr) {
		code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
		throw errors::pointer_is_nullptr(error,READ_ONLY_STRING("base_logger* elm"));
	}
#endif

	return elm;
}
