#include "GameProcess.h"
#include "GameTimer.h"

#include "../MZDX11Renderer/MZDX11Renderer.h"


MZ3DAPI::I3DRenderer* GameProcess::m_pRendererInstance = nullptr;


HRESULT GameProcess::Initialize(HINSTANCE hInstance)
{
	wchar_t szAppName[] = L"MZ Engine Demo 2023";

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = this->WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szAppName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	// 윈도우 클래스 등록
	RegisterClassExW(&wcex);

	// 윈도우 생성
	m_hWnd = CreateWindowW(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		m_windowPosX, m_windowPosY, m_screenWidth, m_screenHeight, nullptr, nullptr, hInstance, NULL);

	if (!m_hWnd)	return S_FALSE;

	// 그래픽스 엔진 초기화
	//m_pRenderer = new MZDX11Renderer();
	m_pRenderer = CreateDX11GraphicsInstance();
	m_pRendererInstance = m_pRenderer;
	m_pRendererInstance->Initialize();
	m_pRendererInstance->SetOutputWindow((UINT)m_hWnd);

	// 생성된 윈도우를 화면에 표시한다.
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	RecalcWindowSize();

	m_pGameTimer = new GameTimer();
	m_pGameTimer->Reset();

	return S_OK;
}


void GameProcess::RecalcWindowSize()
{
	RECT nowRect;
	DWORD _style = (DWORD)GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD _exstyle = (DWORD)GetWindowLong(m_hWnd, GWL_EXSTYLE);

	GetWindowRect(m_hWnd, &nowRect);

	RECT newRect;
	newRect.left = 0;
	newRect.top = 0;
	newRect.right = m_screenWidth;
	newRect.bottom = m_screenHeight;

	AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);

	// 클라이언트 영역보다 윈도우 크기는 더 커야 한다. (외곽선, 타이틀 등)
	int _newWidth = (newRect.right - newRect.left);
	int _newHeight = (newRect.bottom - newRect.top);

	SetWindowPos(m_hWnd, HWND_NOTOPMOST, nowRect.left, nowRect.top,
		_newWidth, _newHeight, SWP_SHOWWINDOW);
}


void GameProcess::Loop()
{
	while (true)
	{
		if (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_msg.message == WM_QUIT)
				break;

			DispatchMessage(&m_msg);
		}
		else
		{
			UpdateAll();
			RenderAll();
		}
	}
}


void GameProcess::UpdateAll()
{
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	// 매 프레임 시간을 계산한다.
	m_pGameTimer->Tick();

	// 렌더러 업데이트
	m_pRendererInstance->Update(m_pGameTimer->DeltaTime());
}


void GameProcess::RenderAll()
{
	m_pRendererInstance->BeginRender();
	m_pRendererInstance->Render();
	m_pRendererInstance->EndRender();
}


void GameProcess::Finalize()
{
	m_pRendererInstance->Finalize();

	ReleaseDX11GraphicsInstance(m_pRenderer);

	delete m_pGameTimer;
}


LRESULT CALLBACK GameProcess::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
		//case WM_LBUTTONDOWN:
		//case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			m_pRendererInstance->OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
			return 0;

			//case WM_LBUTTONUP:
			//case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			m_pRendererInstance->OnMouseUp(LOWORD(lParam), HIWORD(lParam));
			return 0;

		case WM_MOUSEMOVE:
			m_pRendererInstance->OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
