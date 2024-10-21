#pragma once
#include "imgui.h"

/// 어느 창에서든 debug 모드를 제어해서 텍스트를 출력할 수 있도록
/// 2023.08.23 윤종화

class DebugManager
{
private:
	DebugManager();
	~DebugManager();

	static DebugManager* instance;


public:
	static DebugManager* GetInstance();
	static void Delete();

	void DrawDebugText(const char* text, ...);

	const bool GetDebugMode();
	void SetDebugMode(bool tf);

	bool m_isPlayMode = false;

private:
	bool m_isDebugMode;
};
