#pragma once
#include <windows.h>

/// <summary>
/// 간단한 게임 엔진 데모
/// 메인 게임 루프가 진행되는 곳
/// </summary>

namespace MZ3DAPI
{
	class I3DRenderer;
}

class GameTimer;

class GameProcess
{
private:
	HWND m_hWnd;
	MSG m_msg;

	// 윈도우 초기 위치
	const int m_windowPosX = 0;
	const int m_windowPosY = 0;

	// 윈도우 창 크기
	const int m_screenWidth = 1920;
	const int m_screenHeight = 1080;

	// 그래픽스 엔진
	MZ3DAPI::I3DRenderer* m_pRenderer;
	static MZ3DAPI::I3DRenderer* m_pRendererInstance;

	// 시간 제어
	GameTimer* m_pGameTimer;

private:
	void RecalcWindowSize();

	void UpdateAll();
	void RenderAll();

public:
	// 초기화
	HRESULT Initialize(HINSTANCE hInstance);

	// 실제 게임이 진행되는 반복구문
	void Loop();

	// 게임 정리
	void Finalize();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

};

