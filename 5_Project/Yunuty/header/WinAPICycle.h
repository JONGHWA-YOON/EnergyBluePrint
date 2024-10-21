#pragma once
#include <Windows.h>
#include "YunutyCycle.h"

#define MAX_LOADSTRING 100

namespace YunutyEngine
{
    namespace WinAPI
    {
        class WinAPICycle :
            public YunutyCycle
        {
        private:
            static WinAPICycle* _instance;
            HWND hWnd = 0;
            HINSTANCE hInstance = 0;
            WNDCLASSEXW wcex;
            int nCmdShow = 0;
            WNDPROC customWndProc = nullptr;
            WCHAR szTitle[MAX_LOADSTRING] = L"";                  // The title bar text
            WCHAR szWindowClass[MAX_LOADSTRING] = L"";            // the main window class name
            static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
            BOOL InitInstance();
        protected:
            WinAPICycle() {};
            virtual void ThreadUpdate()override;
        public:
            static WinAPICycle& GetInstance();
            BOOL Initialize(HINSTANCE hInstance, const WNDCLASSEXW& wcex, _In_ int nCmdShow, _In_ UINT uID_TITLE, _In_ UINT uID_wndCLASS);
            HWND GetMainWindow();
        };
    }
}
