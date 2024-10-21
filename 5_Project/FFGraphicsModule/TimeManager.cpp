#include "GraphicsPch.h"
#include "TimeManager.h"

void TimeManager::SetDeltaTime(float p_deltaTime)
{
	m_deltaTime = p_deltaTime;
	m_accTIme += m_deltaTime;
}
