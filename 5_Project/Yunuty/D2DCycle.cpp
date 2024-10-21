#include "YunutyEngine.h"
#include "YunuD2DGraphicCore.h"
#pragma comment(lib,"d2d1.lib")


using namespace YunutyEngine;
using namespace YunutyEngine::D2D;

D2DCycle* YunutyEngine::D2D::D2DCycle::_instance = nullptr;

BOOL YunutyEngine::D2D::D2DCycle::InitInstance()
{
    if (!hInstance)
        return false;

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    //HWND hWnd = CreateWindowW(szWindowClass, 0, WS_POPUP,
    //    0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }
    YunuD2D::YunuD2DGraphicCore::GetInstance()->Initialize(hWnd);
    RECT rc;
    GetClientRect(hWnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(
        rc.right - rc.left,
        rc.bottom - rc.top);

    //D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &D2DGraphic::direct2DFactory);

    //D2DGraphic::direct2DFactory->CreateHwndRenderTarget(
    //    D2D1::RenderTargetProperties(),
    //    D2D1::HwndRenderTargetProperties(hWnd, size),
    //    &D2DGraphic::HwndRenderTarget);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    return TRUE;
}
D2DCycle& YunutyEngine::D2D::D2DCycle::GetInstance()
{
    if (!_instance)
        _instance = new D2DCycle();

    return *_instance;
}
BOOL YunutyEngine::D2D::D2DCycle::Initialize(HINSTANCE hInstance, const WNDCLASSEXW& wcex, _In_ int nCmdShow, _In_ UINT uID_TITLE, _In_ UINT uID_wndCLASS)
{
    this->wcex = wcex;
    this->nCmdShow = nCmdShow;
    this->hInstance = hInstance;
    new D2DInput();

    LoadStringW(hInstance, uID_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, uID_wndCLASS, szWindowClass, MAX_LOADSTRING);

    customWndProc = wcex.lpfnWndProc;
    this->wcex.lpfnWndProc = MainWndProc;
    this->wcex.lpszClassName = szWindowClass;
    RegisterClassExW(&this->wcex);

    return InitInstance();
}
void YunutyEngine::D2D::D2DCycle::ThreadUpdate()
{
    YunutyCycle::ThreadUpdate();
    if (IsGameRunning())
        RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

HWND YunutyEngine::D2D::D2DCycle::GetMainWindow()
{
    return hWnd;
}
LRESULT CALLBACK YunutyEngine::D2D::D2DCycle::MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    D2DCycle::GetInstance().hWnd = hWnd;

    if (D2DCycle::GetInstance().customWndProc)
        D2DCycle::GetInstance().customWndProc(hWnd, message, wParam, lParam);
    if (D2DInput::instance)
        D2DInput::instance->MainWndProc(hWnd, message, wParam, lParam);

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        /*switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }*/
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    case WM_PAINT:
        if (dynamic_cast<D2DCamera*>(D2DCamera::GetMainCamera()))
            dynamic_cast<D2DCamera*>(D2DCamera::GetMainCamera())->Render(hWnd, message, wParam, lParam);
        break;
    case WM_SETFOCUS:
        GetInstance().focused = true;
        break;
    case WM_KILLFOCUS:
        GetInstance().focused = false;
        break;
    case WM_DESTROY:
        //D2DCycle::GetInstance().Stop();
        //YunutyCycle::GetInstance().Release();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
