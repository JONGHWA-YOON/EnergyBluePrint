#include "GameTimer.h"
#include <windows.h>

GameTimer::GameTimer()
	:m_secondsPerCount(0.0), m_deltaTime(0.0), m_prevTime(0), m_currTime(0)
{
	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	m_secondsPerCount = 1.0 / (double)countsPerSecond;
}

float GameTimer::DeltaTime() const
{
	return (float)m_deltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_prevTime = currTime;
}

void GameTimer::Tick()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;
	m_prevTime = m_currTime;

	if (m_deltaTime < 0.0)
	{
		m_deltaTime = 0.0;
	}
}
