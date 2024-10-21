#pragma once
#include <Windows.h>
#include "YunutyCycle.h"

#define MAX_LOADSTRING 100

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif


namespace YunutyEngine
{
    namespace D2D
    {
        class YUNUTY_API D2DCycle :
            public YunutyCycle
        {
        private:
            static D2DCycle* _instance;
            bool focused;
            HWND hWnd = 0;
            HINSTANCE hInstance = 0;
            WNDCLASSEXW wcex;
            int nCmdShow = 0;
            WNDPROC customWndProc = nullptr;
            WCHAR szTitle[MAX_LOADSTRING] = L"";                  // The title bar text
            WCHAR szWindowClass[MAX_LOADSTRING] = L"";            // the main window class name
            static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
            BOOL InitInstance();
        protected:
            D2DCycle() {};
            virtual void ThreadUpdate()override;
        public:
            static D2DCycle& GetInstance();
            BOOL Initialize(HINSTANCE hInstance, const WNDCLASSEXW& wcex, _In_ int nCmdShow, _In_ UINT uID_TITLE, _In_ UINT uID_wndCLASS);
            bool isWindowFocused() { return focused; };
            HWND GetMainWindow();
        };
    }
}
