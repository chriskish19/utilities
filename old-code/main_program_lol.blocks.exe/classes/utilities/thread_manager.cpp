#include "thread_manager.hpp"

utilities::thread_master::~thread_master()
{
	for (auto pair : m_thread_p_id_mp) {
		std::jthread* t = pair.second;
		if (t != nullptr) {
			delete t;
		}
	}

	if (m_master_thread != nullptr) {
		delete m_master_thread;
	}
}
