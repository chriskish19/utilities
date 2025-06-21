#include "keyboard_sys.hpp"

errors::codes utilities::keyboard::add_event(key_event kp)
{
	m_key_queue.emplace(kp);
	return errors::codes::success;
}

errors::codes utilities::keyboard::system_message_handler()
{
	key_event current_event = m_key_queue.front();
	int key_code = static_cast<int>(current_event.key);
	switch (key_code) {
	case 'a':
		// move playable character foward
		break;

	default:
		m_key_queue.pop();
		return errors::codes::unknown_keyboard_key_in_system_message_handler;
	}
	m_key_queue.pop();
	return errors::codes::success;
}
