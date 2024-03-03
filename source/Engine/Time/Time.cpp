#include "Time.hpp"

J_SINGLETON_DEF(TimeManager);

TimeManager::TimeManager() :
	m_last(0), m_now(0), m_delta(0) {}
TimeManager::~TimeManager() {}

void TimeManager::updateTime() {
	// Update time.
	m_last = m_now;
	m_now = glfwGetTime();
	m_delta = m_now - m_last;

	// Clamp delta (1 FPS min).
	if (m_delta > 1.0) m_delta = 1.0f;
}