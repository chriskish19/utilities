#include "timer.hpp"

utilities::timer::timer()
{
	m_last = std::chrono::steady_clock::now();
}

float utilities::timer::peek()
{
	return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_last).count();
}
