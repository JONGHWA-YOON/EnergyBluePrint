#include "DebugManager.h"
#include <iostream>

DebugManager* DebugManager::instance = nullptr;

DebugManager::DebugManager()
	: m_isDebugMode(false)
{

}

DebugManager::~DebugManager()
{

}

DebugManager* DebugManager::GetInstance()
{
	if (instance == nullptr)
	{
		instance = new DebugManager();
	}
	return instance;
}

void DebugManager::Delete()
{
	delete instance;
}

void DebugManager::DrawDebugText(const char* text, ...)
{
	if (m_isDebugMode == true)
	{
		va_list args;
		va_start(args, text);

		char buffer[256];
		vsnprintf(buffer, sizeof(buffer), text, args);

		va_end(args);

		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", buffer);
	}
}

const bool DebugManager::GetDebugMode()
{
	return m_isDebugMode;
}

void DebugManager::SetDebugMode(bool tf)
{
	m_isDebugMode = tf;
}