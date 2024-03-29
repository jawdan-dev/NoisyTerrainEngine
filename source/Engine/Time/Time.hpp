#pragma once
#include <Engine/Core/Core.hpp>

class TimeManager {
	J_SINGLETON(TimeManager)
#	define Time J_SINGLETON_GET(TimeManager)

private:
	float m_last, m_now, m_delta;

public:
	TimeManager();
	~TimeManager();

public:
	J_GETTER_DIRECT(now, m_now, float);
	J_GETTER_DIRECT(last, m_last, float);
	J_GETTER_DIRECT(delta, m_delta, float);

public:
	void updateTime();
};