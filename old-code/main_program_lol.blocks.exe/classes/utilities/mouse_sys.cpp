#include "mouse_sys.hpp"

utilities::mouse::mouse_event::mouse_event()
{
#if ENABLE_ALL_EXCEPTIONS
	if (GetCursorPos(&m_pt) == FALSE) {
		throw errors::win32_error();
	}
	m_mouse_window_handle = WindowFromPoint(m_pt);
	if (m_mouse_window_handle == nullptr) {
		code_error_objs::code_obj error(code_error_objs::pointer_is_nullptr);
		throw errors::pointer_is_nullptr(error,READ_ONLY_STRING("m_mouse_window_handle = WindowFromPoint(m_pt)"));
	}
#endif	

#if ENABLE_FULL_OPTIMIZATIONS
	GetCursorPos(&m_pt);
	m_mouse_window_handle = WindowFromPoint(m_pt);
#endif
}

errors::codes utilities::mouse::add_mouse_event(const mouse_event& me)
{
	m_me_queue.emplace(me);
	return errors::codes::success;
}

errors::codes utilities::mouse::system_message_handler()
{
	mouse_event me = m_me_queue.front();
	switch (me.get_mouse_button()) {
	case mouse_buttons::left_click:

		break;
	case mouse_buttons::right_click:

		break;
	case mouse_buttons::scroll_wheel:

		break;

	default:
		return errors::codes::unknown_mouse_button_in_system_message_handler;
	}
	return errors::codes::success;
}
